#pragma once

#include "Node.h"

#include "Camera.h"
#include "Geometry.h"
#include "AnimCurve.h"
#include "Material.h"

namespace Pimp 
{
	class Overlay : public Element
	{
	private:
		float startTime; // time at which we'll start fading in
		float endTime; // time at which we've faded out
		float fadeInRate; // amount per second
		float fadeOutRate; // amount per second

		float opacity, progress;
		Material* material;

		void UpdateOpacity();

	public:
		Overlay(World* ownerWorld);
		virtual ~Overlay() {}

		void Tick(float deltaTime);
		void Render(Camera* camera);

		void SetMaterial(Material* mat)
		{
			material = mat;
		}

		Material* GetMaterial() const
		{
			return material;
		}

		void SetTimings(float newStartTime, float newEndTime, float newFadeInRate, float newFadeOutRate);

		virtual void ForceSetTime(float time);
	};
}