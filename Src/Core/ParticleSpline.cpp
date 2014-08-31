#include "ParticleSpline.h"
#include "ElementTypes.h"
#include "D3D.h"
#include "World.h"
#include "ParticleAttractor.h"

#define PIMP_PARTICLESPLINE_MAXPARTICLES 64000
#define PIMP_SPLINE_SAMPLEDDISTANCE_ENTRIES 1024

#define PIMP_PARTICLESPLINE_ORIENTSAMPLEDIST 0.1f

namespace Pimp
{
	//
	// Spline
	// 

	Spline::Spline(const Vector4* CVs, const float* knots, int numCVs, int numKnots)
		: numCVs(numCVs), numKnots(numKnots), distanceSamples(PIMP_SPLINE_SAMPLEDDISTANCE_ENTRIES)
	{
		this->CVs = new Vector4[numCVs];
		this->knots = new float[numKnots];

		memcpy(this->CVs, CVs, sizeof(Vector4)*numCVs);
		memcpy(this->knots, knots, sizeof(float)*numKnots);

		// Generate sampledDistance table
		
		FixedSizeList<DistanceSample> timeToDistanceSamples(PIMP_SPLINE_SAMPLEDDISTANCE_ENTRIES*4);

		const float tstep = 1.0f / (float)(timeToDistanceSamples.SizeAllocated()-1);
		float totalDistance = 0;

		Vector3 prevPos = SampleAtTime(0);

		for (float t=0; t<=1; t+=tstep)
		{
			Vector3 currentPos = SampleAtTime(t);

			float distance = (currentPos - prevPos).Length();
			
			totalDistance += distance;

			DistanceSample newSample;
			newSample.time = t;
			newSample.distance = totalDistance;

			timeToDistanceSamples.Add(newSample);

			prevPos = currentPos;
		}

		totalLength = totalDistance;

		// Now build an inverse-table, so we can map efficiently from distance to time!
		int offset = 0;
		for (int i=0; i<PIMP_SPLINE_SAMPLEDDISTANCE_ENTRIES; ++i)
		{
			float distance = ((float)i / (float)(PIMP_SPLINE_SAMPLEDDISTANCE_ENTRIES-1)) * totalDistance;
			float t = 1;

			// Crawl through our list as long as we're further than our c
			while ( (offset < timeToDistanceSamples.Size()-1) &&				
					distance >= timeToDistanceSamples[offset+1].distance )
			{
				++offset;
			}

			if (offset < timeToDistanceSamples.Size()-1)
			{
				// linear interpolate
				float alpha = 0;
				float delta = timeToDistanceSamples[offset + 1].distance - timeToDistanceSamples[offset].distance;

				if (delta > 0)
				{
					alpha = (distance - timeToDistanceSamples[offset].distance) / delta;
				}

				t = 
					timeToDistanceSamples[offset].time * (1 - alpha) + 
					timeToDistanceSamples[offset+1].time * alpha;
			}

			DistanceSample newSample;
			newSample.time = t;
			newSample.distance = distance;

			distanceSamples.Add(newSample);
		}

		Vector3 dummy;

		// Now calculate orientations (very inefficiently! :-))
		for (int i=0; i<distanceSamples.Size(); ++i)
		{
			DistanceSample& sample = distanceSamples[i];

			Vector3 forward = 
				SampleAtTime(FindTimeAndOrientationForGivenDistance(sample.distance + PIMP_PARTICLESPLINE_ORIENTSAMPLEDIST, &dummy, &dummy)) - 
				SampleAtTime(FindTimeAndOrientationForGivenDistance(sample.distance - PIMP_PARTICLESPLINE_ORIENTSAMPLEDIST, &dummy, &dummy));

			float forwardDist = forward.x*forward.x + forward.y*forward.y + forward.z*forward.z;

			if (forwardDist < 0.001f*0.001f && i>0)
			{
				sample.side = distanceSamples[i-1].side;
				sample.up = distanceSamples[i-1].up;
				continue;
			}

			Vector3 fixedUp(0,1,0);

			float dotangle = fixedUp.Dot(forward);

			if (dotangle > 0.7f || dotangle < -0.7f)
			{
				fixedUp.y = 0;
				fixedUp.z = 1;
			}

			sample.side = forward.Cross(fixedUp).Normalized();
			sample.up = sample.side.Cross(forward).Normalized();
		}
	}


	float Spline::FindTimeAndOrientationForGivenDistance(
		float distance, Vector3* outSide, Vector3* outUp) const
	{
		distance /= totalLength;
		distance = min(max(distance, 0), 1);

		float sampleIndex = distance * (float)(PIMP_SPLINE_SAMPLEDDISTANCE_ENTRIES-1);
		int sampleIndexInt = (int)floorf(sampleIndex);

		if (sampleIndex <= 0.0f)
		{
			*outSide = distanceSamples[0].side;
			*outUp = distanceSamples[0].up;
			return 0;
		}
		if (sampleIndexInt >= PIMP_SPLINE_SAMPLEDDISTANCE_ENTRIES-1)
		{
			*outSide = distanceSamples[PIMP_SPLINE_SAMPLEDDISTANCE_ENTRIES-1].side;
			*outUp = distanceSamples[PIMP_SPLINE_SAMPLEDDISTANCE_ENTRIES-1].up;
			return 1;
		}

		const DistanceSample& s1 = distanceSamples[sampleIndexInt];
		const DistanceSample& s2 = distanceSamples[sampleIndexInt+1];

		float alpha = sampleIndex - (float)sampleIndexInt;

		ASSERT(alpha >= 0 && alpha < 1);

		*outSide = Lerp(s1.side, s2.side, alpha).Normalized();
		*outUp = Lerp(s1.up, s2.up, alpha).Normalized();

		return s1.time * (1-alpha) + s2.time * alpha;
	}


	Vector3 Spline::SampleAtDistance(float distance, Vector3* outSide, Vector3* outUp) const
	{
		return SampleAtTime(FindTimeAndOrientationForGivenDistance(distance, outSide, outUp));
	}

	float Spline::funcTime(int i) const
	{
		if (i < 0)
			return 0;
		else
			if (i >= numKnots)
				return knots[numKnots-1];
			else
				return knots[i];
	}


	// i = point index
	// k = order = degree+1
	float Spline::funcN(float t, int i, int k) const
	{
		if (k == 1)
		{
			if ((t >= funcTime(i)) && (t <= funcTime(i+1)))
				return 1;
			else
				return 0;
		} else
		{
			float firstDivider = (funcTime(i+k-1) - funcTime(i));
			float secondDivider = (funcTime(i+k) - funcTime(i+1));

			float result = 0;

			if (firstDivider != 0)
				result += ((t - funcTime(i)) / firstDivider) * funcN(t, i, k-1);         

			if (secondDivider != 0)
				result += ((funcTime(i+k) - t) / secondDivider) * funcN(t, i+1, k-1);      

			return result;
		}
	}

	Vector3 Spline::SampleAtTime(float time) const
	{
		if (time < 0.00001f)
			time = 0.00001f;
		else if (time > 1)
			time = 1;

		// convert to knots-space
		time *= knots[numKnots-1];

		Vector3 totalWeightedPosition(0,0,0);
		float totalWeight = 0;

		for (int i=0; i<numCVs; i++)
		{
			float N = funcN(time, i, 4); // sample, 4th order (degree 3)
			float w = CVs[i].w;

			Vector3 pos(CVs[i].x, CVs[i].y, CVs[i].z);

			totalWeightedPosition += pos * N*w;
			totalWeight += N*w;
		}

		if (totalWeight != 0)
			totalWeightedPosition /= totalWeight;

		return totalWeightedPosition;
	}


	//
	// ParticleSpline
	// 

	ParticleSpline::ParticleSpline(Pimp::World *ownerWorld)
		: Node(ownerWorld), 
		particles(PIMP_PARTICLESPLINE_MAXPARTICLES), 
		particleVertices(PIMP_PARTICLESPLINE_MAXPARTICLES*4),
		particleIndices(PIMP_PARTICLESPLINE_MAXPARTICLES*6),
		spline(NULL),
		currentDistanceTraveled(0),
		particleSizeMin(0.1f),
		particleSizeMax(0.2f),
		cylRadius(0.1f),
		cylLength(1.0f),
		numParticles(16),
		animCurveDistanceTraveled(NULL),
		spinSpeedRandomPercentage(0),
		spinSpeedMax(0),
		animCurveSpinSpeedMax(NULL),
		visibleInterval(0, 99999999.0f)
	{
		SetType(ET_ParticleSpline);

		RebuildParticles();
	}

#ifdef _DEBUG
	ParticleSpline::~ParticleSpline()
	{
		delete spline;
	}
#endif


	//void ParticleSpline::Render(Camera* camera)
	//{
	//	if (material != NULL && particles.Size() > 0)
	//	{
	//		float worldTime = ownerWorld->GetCurrentTime();

	//		if (worldTime < visibleInterval.x ||
	//			worldTime > visibleInterval.y)
	//			return;

	//		const Matrix4* viewMatrix = camera->GetViewMatrixPtr();
	//		
	//		Vector3 cameraSide(viewMatrix->_11, viewMatrix->_21, viewMatrix->_31);
	//		Vector3 cameraUp(viewMatrix->_12, viewMatrix->_22, viewMatrix->_32);

	//		viewSpaceOffsets[0] = -cameraSide +  cameraUp;
	//		viewSpaceOffsets[1] =  cameraSide +  cameraUp;
	//		viewSpaceOffsets[2] = -cameraSide + -cameraUp;
	//		viewSpaceOffsets[3] =  cameraSide + -cameraUp;

	//		static const float uvs[] = {
	//			0,0, 
	//			1,0, 
	//			0,1, 
	//			1,1
	//		};


	//		Vertex* outVertex = particleVertices.GetItemsPtr();
	//		WORD* outIndex = particleIndices.GetItemsPtr();

	//		for (int i=0; i<particles.Size(); ++i)
	//		{
	//			float sizesmoother = 1.0f + 0.1f * mySinf(worldTime * 1.9f + i);


	//			for (int j=0; j<4; ++j)
	//			{
	//				outVertex[j].position = 
	//					viewSpaceOffsets[j] * particles[i].size * sizesmoother +
	//					particles[i].currentPosition;

	//				outVertex[j].tex0 = *(Vector2*)&uvs[j*2];
	//				outVertex[j].particleNumber = particles[i].frontness;
	//			}

	//			outVertex += 4;

	//			int baseOffset = i*4;
	//			*(outIndex++) = baseOffset;
	//			*(outIndex++) = baseOffset+1;
	//			*(outIndex++) = baseOffset+3;
	//			*(outIndex++) = baseOffset;
	//			*(outIndex++) = baseOffset+3;
	//			*(outIndex++) = baseOffset+2;
	//		}

	//		material->SetParameters(&Matrix4::IDENTITY, &Matrix4::IDENTITY, camera);
	//		material->Begin();

	//		D3D_VERIFY(D3D::dev->DrawIndexedPrimitiveUP(
	//			D3DPT_TRIANGLELIST, 0, 
	//			particleVertices.Size(), particles.Size()*2,
	//			particleIndices.GetItemsPtr(), D3DFMT_INDEX16, 
	//			particleVertices.GetItemsPtr(), sizeof(Vertex)),
	//			"Error drawing particles");

	//		material->End();
	//	}
	//}	


	void ParticleSpline::SetParameters(float particleSizeMin, float particleSizeMax, 
		float cylRadius, float cylLength, int numParticles, float spinSpeedRandomPercentage)
	{
		this->particleSizeMin = particleSizeMin;
		this->particleSizeMax = particleSizeMax;
		this->cylLength = cylLength;
		this->cylRadius = cylRadius;
		this->numParticles = numParticles;
		this->spinSpeedRandomPercentage = spinSpeedRandomPercentage;

		RebuildParticles();
	}


	void ParticleSpline::SetSpline(const Vector4* CVs, const float* knots, int numCVs, int numKnots)
	{
		delete spline;
		spline = new Spline(CVs, knots, numCVs, numKnots);
	}


	void ParticleSpline::SetCurrentDistanceTraveled(float distance)
	{
		currentDistanceTraveled = distance;
	}

	void ParticleSpline::SetAnimCurveDistanceTraveled(AnimCurve* curve)
	{
		animCurveDistanceTraveled = curve;
	}

	void ParticleSpline::SetSpinSpeedMax(float maxSpinSpeed)
	{
		spinSpeedMax = maxSpinSpeed;
	}

	void ParticleSpline::SetAnimCurveSpinSpeedMax(AnimCurve* curve)
	{
		animCurveSpinSpeedMax = curve;
	}

	// Init
	void ParticleSpline::RebuildParticles()
	{
		static int particlesSeed = 1;

		particles.Clear();
		particleVertices.Clear();
		particleIndices.Clear();

		particles.AddMultipleNoninitialized(numParticles);
		particleVertices.AddMultipleNoninitialized(4*numParticles);
		particleIndices.AddMultipleNoninitialized(6*numParticles);


		for (int i=0; i<numParticles; ++i)
		{
			Particle& p = particles[i];

			float alpha = (float)i / (float)(numParticles-1);

			p.frontness = alpha;
			p.distanceOffset = cylLength * alpha;
			p.size = particleSizeMin + (particleSizeMax-particleSizeMin) * sfrand_normalized(&particlesSeed);
			p.radius = cylRadius * sfrand_normalized(&particlesSeed);
			p.initialPhi = sfrand_normalized(&particlesSeed) * M_PI_2;	
			p.phi = p.initialPhi;
			p.phiSpeed = (1-spinSpeedRandomPercentage) + (sfrand_normalized(&particlesSeed) * spinSpeedRandomPercentage);
		}
	}

	// Behaviour
	void ParticleSpline::Tick(float deltaTime)
	{
		float worldTime = ownerWorld->GetCurrentTime();

		if (worldTime < visibleInterval.x ||
			worldTime > visibleInterval.y)
			return;

		if (animCurveDistanceTraveled != NULL)
			currentDistanceTraveled = animCurveDistanceTraveled->GetCurrentValue();

		if (animCurveSpinSpeedMax != NULL)
			spinSpeedMax = animCurveSpinSpeedMax->GetCurrentValue();

		if (spline != NULL)
		{
			const FixedSizeList<ParticleAttractor*>& attractors = ownerWorld->GetParticleAttractors();

			ParticleAttractor* attractor = (attractors.Size() > 0) ? attractors[0] : NULL;
			bool blendToAttractor;

			if (attractor != NULL)
			{
				if (attractor->GetIntensity() <= 0)
					attractor = NULL;
				else 
					blendToAttractor = attractor->GetIntensity() < 1;
			}
			else
			{
				blendToAttractor = false;
			}


			// Calculate new positions for all particles.

			if (attractor == NULL || blendToAttractor)
			{
				for (int i=0; i<particles.Size(); ++i)
				{
					Particle& p = particles[i];

					float distance = currentDistanceTraveled + p.distanceOffset;

					Vector3 side, up;

					p.currentPosition = spline->SampleAtDistance(distance, &side, &up);
					p.phi = p.initialPhi + p.phiSpeed * spinSpeedMax;

					float cx = cosf(p.phi) * p.radius;
					float cy = sinf(p.phi) * p.radius;

					side *= cx;
					up *= cy;
										
					p.currentPosition += side;
					p.currentPosition += up;
				}			
			}

			if (attractor != NULL)
			{
				float blendFactor = attractor->GetIntensity();

				// Pull towards attractor!
				const Vector3& attractorPos = attractor->GetWorldPosition();
				float r = attractor->GetRadius();

				float t = 0;
				float tstep = M_PI / (float)particles.Size();
				
				for (int i=0; i<particles.Size(); ++i)
				{
					Particle& p = particles[i];

					float scaleT = sinf(t);

					Vector3 spherePos(
						r * cosf(p.phi) * scaleT + attractorPos.x,
						r * sinf(p.phi) * scaleT + attractorPos.y,
						-r * cosf(t) + attractorPos.z
					);


					p.currentPosition = Lerp(p.currentPosition, spherePos, blendFactor);

					t += tstep;
				}
			}
		}

		Node::Tick(deltaTime);
	}

	void ParticleSpline::ForceSetTime(float time)
	{
		Node::ForceSetTime(time);

	}

	void ParticleSpline::SetVisibleInterval(const Vector2& interval)
	{
		visibleInterval = interval;
	}
}
