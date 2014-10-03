#pragma once

#include "Node.h"
#include "Camera.h"
#include "AnimCurve.h"

namespace Pimp 
{
	class ParticleAttractor
		: public Node
	{
	private:
		float intensity, smoothedIntensity;
		AnimCurve* animCurveIntensity;
		AnimCurve* animCurveRadius;
		AnimCurve* animCurveRadius2;
		Vector3 worldPosition;

		float radius, radius2;

	public:
		ParticleAttractor(World* ownerWorld);
		virtual ~ParticleAttractor();

		void Tick(float deltaTime);

		void SetIntensity(float intensity);
		void SetAnimCurveIntensity(AnimCurve* animCurveIntensity);

		void SetRadius(float radius);
		void SetAnimCurveRadius(AnimCurve* animCurveRadius);
		void SetRadius2(float radius2);
		void SetAnimCurveRadius2(AnimCurve* animCurveRadius2);

		float GetIntensity() const { return smoothedIntensity; }
		float GetRadius() const { return radius*radius2; }
		const Vector3& GetWorldPosition() { return worldPosition; }
	};
}