#include "ParticleAttractor.h"
#include "World.h"

namespace Pimp
{
	ParticleAttractor::ParticleAttractor(World* ownerWorld)
		: Node(ownerWorld), animCurveIntensity(NULL),
		animCurveRadius(NULL), animCurveRadius2(NULL),
		intensity(1), radius(1), radius2(1)
	{
		SetType(ET_ParticleAttractor);
		ownerWorld->RegisterParticleAttractor(this);
	}

	ParticleAttractor::~ParticleAttractor()
	{
		ownerWorld->UnregisterParticleAttractor(this);
	}

	void ParticleAttractor::SetIntensity(float intensity)
	{
		this->intensity = intensity;
	}

	void ParticleAttractor::SetAnimCurveIntensity(AnimCurve* curve)
	{
		animCurveIntensity = curve;
	}

	void ParticleAttractor::Tick(float deltaTime)
	{
		if (animCurveIntensity != NULL)
		{
			intensity = animCurveIntensity->GetCurrentValue();
		}

		if (animCurveRadius != NULL)
		{
			radius = animCurveRadius->GetCurrentValue();
		}

		if (animCurveRadius2 != NULL)
		{
			radius2 = animCurveRadius2->GetCurrentValue();
		}

		const Matrix4* world = Node::GetWorldTransform();

		worldPosition.x = world->m[3][0];
		worldPosition.y = world->m[3][1];
		worldPosition.z = world->m[3][2];

		// Smoothed lerp, s-curved.
		smoothedIntensity = intensity * intensity * (3.0f - 2.0f*intensity);

		//radiusSq = radius*radius*radius2*radius2;

		Node::Tick(deltaTime);
	}

	void ParticleAttractor::SetRadius(float radius)
	{
		this->radius = radius;
	}

	void ParticleAttractor::SetAnimCurveRadius(AnimCurve* animCurveRadius)
	{
		this->animCurveRadius = animCurveRadius;
	}

	void ParticleAttractor::SetRadius2(float radius2)
	{
		this->radius2 = radius2;
	}

	void ParticleAttractor::SetAnimCurveRadius2(AnimCurve* animCurveRadius2)
	{
		this->animCurveRadius2 = animCurveRadius2;
	}
}