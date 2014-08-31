#pragma once

#include "Node.h"
#include "Camera.h"
#include "Geometry.h"
#include "AnimCurve.h"


namespace Pimp 
{
	class Spline
	{
	private:
		Vector4* CVs;
		float* knots;
		int numKnots;
		int numCVs;

		float totalLength;

		struct DistanceSample
		{
			float time;
			float distance;
			Vector3 side; 
			Vector3 up;
		};

		FixedSizeList<DistanceSample> distanceSamples;

		float FindTimeAndOrientationForGivenDistance(
			float distance, Vector3* outSide, Vector3* outUp) const;
		float funcTime(int i) const;
		float funcN(float t, int i, int k) const;

	public:
		Spline(const Vector4* CVs, const float* knots, int numCVs, int numKnots);		

		Vector3 SampleAtTime(float time) const;
		Vector3 SampleAtDistance(float distance, Vector3* outSide, Vector3* outUp) const;
	};

	class ParticleSpline
		: public Node
	{
	private:
		struct Particle
		{
			Vector3 currentPosition;

			float distanceOffset;
			float size;
			float radius;
			float phi, initialPhi;
			float phiSpeed;
			float frontness; // 1 for front, 0 for tail
		};

		struct Vertex
		{
			Vector3 position;
			float particleNumber;
			Vector2 tex0;
		};

		FixedSizeList<Particle> particles;
		FixedSizeList<Vertex> particleVertices;
		FixedSizeList<WORD> particleIndices;

		float particleSizeMin;
		float particleSizeMax;
		float cylRadius;
		float cylLength;
		int numParticles;
		float currentDistanceTraveled;
		AnimCurve* animCurveDistanceTraveled;

		float spinSpeedRandomPercentage;
		float spinSpeedMax;
		AnimCurve* animCurveSpinSpeedMax;

		Vector3 viewSpaceOffsets[4];
		Spline* spline;

		Vector2 visibleInterval;

		void RebuildParticles();

	public:
		ParticleSpline(World* ownerWorld);
#ifdef _DEBUG
		~ParticleSpline();
#endif

		void Tick(float deltaTime);

		void SetParameters(float particleSizeMin, float particleSizeMax, 
			float cylRadius, float cylLength, int numParticles, float spinSpeedRandomPercentage);

		void SetSpline(const Vector4* CVs, const float* knots, int numCVs, int numKnots);

		void SetCurrentDistanceTraveled(float distance);		
		void SetAnimCurveDistanceTraveled(AnimCurve* curve);

		void SetSpinSpeedMax(float maxSpinSpeed);
		void SetAnimCurveSpinSpeedMax(AnimCurve* curve);

		void SetVisibleInterval(const Vector2& interval);

		void ForceSetTime(float time);

	};
}