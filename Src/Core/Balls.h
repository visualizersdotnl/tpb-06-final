#pragma once

#include "AnimCurve.h"
#include "Node.h"
#include <shared/shared.h>
#include "Effect.h"

#ifdef _DEBUG

namespace Pimp 
{
	class Xform;

	class Balls : public Node
	{
	public:
		struct BoundBalls
		{
			int varIndexBallOffsetsAndRadii; // list of float4's. xyz=offset, w=radius
			Balls* balls;

			BoundBalls() : balls(NULL) {}
		};

	private:
		struct BallCachedAnim
		{
			AnimCurve* animCurvePosX; // in world space
			AnimCurve* animCurvePosY; // in world space
			AnimCurve* animCurvePosZ; // in world space
			float radius;
		};

		FixedSizeList<BallCachedAnim> cachedBallAnims;
		FixedSizeList<Vector4> cachedBallCurrentOffsets;

		float systemRadius;
		Vector2 ballRadiusRange; //x=min,y=max
		Vector2 ballSpeedRange;
		Vector2 simulationTimeRange; // x=min, y=max (in seconds)		

		char name[512];

	public:
		Balls(World* ownerWorld);		

#ifdef _DEBUG
		~Balls();
#endif

		const char* GetName() const 
		{ 
			return name; 
		}

		void SetName(const char* newName);
		void SetParams(float newSystemRadius, float newBallRadiusMin, float newBallRadiusMax, float newBallSpeedMin, float newBallSpeedMax, float startTime, float endTime);

		void CalculateCachedBallAnims();

		void AssignValueToEffectVariables(Effect* effect, const BoundBalls& varIndices);
	};
}

#endif