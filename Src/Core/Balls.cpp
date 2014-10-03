#include "Balls.h"
#include "World.h"
#include "Xform.h"

#ifdef _DEBUG


#define PIMP_BALLS_COUNT 64

namespace Pimp
{
	Balls::Balls(World* ownerWorld)
		: Node(ownerWorld),
		cachedBallAnims(32*6),
		cachedBallCurrentOffsets(32*6),
		systemRadius(1.0f),
		ballRadiusRange(Vector2(0.05f, 0.10f)),
		simulationTimeRange(Vector2(0.0f, 1.0f)),
		ballSpeedRange(Vector2(0.01f, 0.05f))
	{
		SetType(ET_Balls);
		name[0] = 0;

#ifdef _DEBUG
		CalculateCachedBallAnims();
#endif
	}

	Balls::~Balls()
	{
		for (int i=0; i<cachedBallAnims.Size(); ++i)
		{
			delete cachedBallAnims[i].animCurvePosX;
			delete cachedBallAnims[i].animCurvePosY;
			delete cachedBallAnims[i].animCurvePosZ;
		}
	}

	void Balls::SetName(const char* newName)
	{
		strcpy(name, newName);
	}

	void Balls::SetParams(float newSystemRadius, float newBallRadiusMin, float newBallRadiusMax, float newBallSpeedMin, float newBallSpeedMax, float startTime, float endTime)
	{
		systemRadius = newSystemRadius;
		ballRadiusRange.x = newBallRadiusMin;
		ballRadiusRange.y = newBallRadiusMax;
		simulationTimeRange.x = startTime;
		simulationTimeRange.y = endTime;
		ballSpeedRange.x = newBallSpeedMin;
		ballSpeedRange.y = newBallSpeedMax;

		CalculateCachedBallAnims();
	}

	static Vector3 GetRandomNormalVector(int* randSeed)
	{
		Vector3 pos;
		pos.x = sfrand(randSeed);
		pos.y = sfrand(randSeed);
		pos.z = sfrand(randSeed);
		pos.Normalize();

		return pos;
	}


	void Balls::CalculateCachedBallAnims()
	{
		for (int i=0; i<cachedBallAnims.Size(); ++i)
		{
			delete cachedBallAnims[i].animCurvePosX;
			delete cachedBallAnims[i].animCurvePosY;
			delete cachedBallAnims[i].animCurvePosZ;
		}

		cachedBallAnims.Clear();

		const Matrix4* xform = GetWorldTransform();

		// First determine time range to simulate
		float simulateTimeStart = simulationTimeRange.x;
		float simulateTimeStop = simulationTimeRange.y;

#ifdef _DEBUG
		if (simulateTimeStop <= simulateTimeStart)
			return;
#endif

		const float simulateTimeStep = 1/30.0f; // Sample our physics at 30Hz

		simulateTimeStop += simulateTimeStep;
		int numSimulationSteps = (int)floorf((simulateTimeStop - simulateTimeStart) / simulateTimeStep);

		// Pre-allocate work buffer for position info
		// (store an anim of numSimulationSteps keys, and one such curve per component (X,Y or Z) and for each ball)
		AnimCurve::Pair** simulationKeys = new AnimCurve::Pair*[3*PIMP_BALLS_COUNT];
		for (int i=0; i<3*PIMP_BALLS_COUNT; ++i)
			simulationKeys[i] = new AnimCurve::Pair[numSimulationSteps];

		struct BallState
		{
			Vector3 currentPositionWorld;
			Vector3 basisX, basisY;
			float time;
			float speed;

			Vector3 velocityWorld;
			float radius;
		};

		BallState* ballStates = new BallState[PIMP_BALLS_COUNT];
		memset(ballStates, 0, PIMP_BALLS_COUNT*sizeof(BallState));

		int randSeed = 1337;

		// Perform our physics simulation!
		float simulateTime = simulateTimeStart;
		for (int simulationStep=0; simulationStep<numSimulationSteps; ++simulationStep)
		{
			if (simulationStep == 0)
			{
				for (int ballIndex=0; ballIndex<PIMP_BALLS_COUNT; ++ballIndex)
				{
					BallState& ballState = ballStates[ballIndex];
			
					Vector3 other = GetRandomNormalVector(&randSeed);
					ballState.basisX = GetRandomNormalVector(&randSeed);
					ballState.basisY = other.Cross(ballState.basisX).Normalized();

					ballState.time = sfrand_normalized(&randSeed) * M_PI_2;
					ballState.speed = ballSpeedRange.x + (ballSpeedRange.y-ballSpeedRange.x)*sfrand_normalized(&randSeed);
					
					ballState.radius = ballRadiusRange.x + (ballRadiusRange.y - ballRadiusRange.x)*sfrand_normalized(&randSeed);
				}
			}

			for (int ballIndex=0; ballIndex<PIMP_BALLS_COUNT; ++ballIndex)
			{
				BallState& ballState = ballStates[ballIndex];

				// Circular motion
				Vector3 pos = ballState.basisX * cosf(ballState.time) + ballState.basisY * sinf(ballState.time);
				pos *= systemRadius;

				ballState.currentPositionWorld = xform->TransformCoord(pos);

				ballState.time += ballState.speed;

				// Store time
				for (int i=0; i<3; ++i)
					simulationKeys[ballIndex*3+i][simulationStep].time = simulateTime;

				// Store pos
				simulationKeys[ballIndex*3+0][simulationStep].value = ballState.currentPositionWorld.x;
				simulationKeys[ballIndex*3+1][simulationStep].value = ballState.currentPositionWorld.y;
				simulationKeys[ballIndex*3+2][simulationStep].value = ballState.currentPositionWorld.z;
			}

			simulateTime += simulateTimeStep;
		}

		// Build anim curves for our results
		for (int allBallsIndex=0; allBallsIndex<PIMP_BALLS_COUNT; ++allBallsIndex)
		{
			AnimCurve* newCurves[3];

			for (int compIndex=0; compIndex<3; ++compIndex)
			{
				AnimCurve* newCurve = new AnimCurve(ownerWorld);
				newCurves[compIndex] = newCurve;

				newCurve->SetInterpolationType(AnimCurve::InterpolationType_Hold);
				newCurve->SetKeysPtr(simulationKeys[allBallsIndex*3+compIndex], numSimulationSteps);
			}

			BallCachedAnim anim;
			anim.radius = ballStates[allBallsIndex].radius;
			anim.animCurvePosX = newCurves[0];
			anim.animCurvePosY = newCurves[1];
			anim.animCurvePosZ = newCurves[2];

			cachedBallAnims.Add(anim);
		}

		for (int i=0; i<PIMP_BALLS_COUNT; ++i)
			delete [] simulationKeys[i];

		delete [] ballStates;
		delete [] simulationKeys;

	}

	void Balls::AssignValueToEffectVariables(Effect* effect, const BoundBalls& varIndices)
	{
		// Bind our variables! Don't forget to verify variable types!

#ifdef _DEBUG
		D3D10_EFFECT_TYPE_DESC desc;

		desc = effect->GetTypeDesc(varIndices.varIndexBallOffsetsAndRadii);

		if (desc.Elements != PIMP_BALLS_COUNT)
			return;

		if (cachedBallAnims.IsEmpty())
			return;
#endif

		cachedBallCurrentOffsets.Clear();
		cachedBallCurrentOffsets.AddMultipleNoninitialized(cachedBallAnims.Size());

		for (int i=0; i<cachedBallAnims.Size(); ++i)
		{
			cachedBallCurrentOffsets[i].x = cachedBallAnims[i].animCurvePosX->GetValueAt(ownerWorld->GetCurrentTime());
			cachedBallCurrentOffsets[i].y = cachedBallAnims[i].animCurvePosY->GetValueAt(ownerWorld->GetCurrentTime());
			cachedBallCurrentOffsets[i].z = cachedBallAnims[i].animCurvePosZ->GetValueAt(ownerWorld->GetCurrentTime());
			cachedBallCurrentOffsets[i].w = cachedBallAnims[i].radius;
		}

		// offsets
		effect->SetVariableValue(varIndices.varIndexBallOffsetsAndRadii, cachedBallCurrentOffsets.GetItemsPtr(), cachedBallCurrentOffsets.Size());
	}
}

#endif