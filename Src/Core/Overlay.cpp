#include "Overlay.h"
#include "ElementTypes.h"
#include "Configuration.h"
#include "D3D.h"
#include "World.h"

namespace Pimp
{
	Overlay::Overlay(World* ownerWorld)
		: Element(ownerWorld),
		opacity(0), progress(0)
	{
		SetType(ET_Overlay);
	}

	void Overlay::Tick(float deltaTime)
	{
		UpdateOpacity();

		Element::Tick(deltaTime);
	}

	void Overlay::Render(Camera* camera)
	{
		if (opacity > 0.0f && material != NULL)
		{
			material->SetOverlayOpacityAndProgress(opacity, progress, max(0.0f, ownerWorld->GetCurrentTime() - startTime));
			material->Bind(camera);
			gD3D->DrawTriQuad(0);
		}
	}

	void Overlay::ForceSetTime( float time )
	{
		Element::ForceSetTime(time);

		UpdateOpacity();
	}

	void Overlay::UpdateOpacity()
	{
		float time = ownerWorld->GetCurrentTime();

		if (time >= startTime && time <= endTime)
		{
			opacity = 
				Clamp(
					Clamp((time-startTime)*fadeInRate, 0.0f, 1.0f) +
					Clamp((endTime-time)*fadeOutRate, 0.0f, 1.0) - 1.0f,
					0.0f, 1.0f);

			progress = (time - startTime) / (endTime - startTime);
		}
		else
		{
			opacity = 0.0f;
			progress = 0.0f;
		}
	}

	void Overlay::SetTimings( float newStartTime, float newEndTime, float newFadeInRate, float newFadeOutRate )
	{
		startTime = newStartTime;
		endTime = newEndTime;
		fadeInRate = newFadeInRate;
		fadeOutRate = newFadeOutRate;

		UpdateOpacity();
	}
}