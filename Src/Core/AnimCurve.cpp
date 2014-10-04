
#include "Settings.h"
#include "AnimCurve.h"
#include "World.h"

namespace Pimp
{
	AnimCurve::AnimCurve(Pimp::World *ownerWorld)
		: Element(ownerWorld), keysPtr(new FixedSizeList<Pair>(0)),
		currentTime(0), currentValue(0),
		interpolationType(InterpolationType_Tangents)
	{
		SetType(ET_AnimCurve);
	}

	AnimCurve::~AnimCurve() {}

	void AnimCurve::SetKeysPtr(const Pair* pairs, int numPairs)
	{
		keysPtr = new FixedSizeList<Pair>(numPairs);
		keysPtr->AddMultiple(pairs, numPairs);
	}

	void AnimCurve::ForceSetTime(float time)
	{
		currentTime = time;
	}


	float AnimCurve::GetValueAt(float time) const
	{
#if 1
		// Completely inefficient key searching method!
		// 
		// Should only be used in Maya. This method handles
		// unordered keys.

		if (keysPtr->Size() == 0)
			return 0.0f;

		bool noLeftKey = true;
		bool noRightKey = true;
		int leftKeyIndex, rightKeyIndex;
		float leftTime, rightTime;

		const Pair* finger = keysPtr->GetItemsPtr();
		for (int i=0; i<keysPtr->Size(); ++i, ++finger)
		{
			if (finger->time <= time)
			{
				// Left of our time

				if (noLeftKey)
				{
					leftKeyIndex = i;
					leftTime = finger->time;
					noLeftKey = false;
				} else if (finger->time > leftTime)
				{
					leftTime = finger->time;
					leftKeyIndex = i;
				}
			} else 
			{
				// Right of our time

				if (noRightKey)
				{
					rightKeyIndex = i;
					rightTime = finger->time;
					noRightKey = false;
				} else if (finger->time < rightTime)
				{
					rightTime = finger->time;
					rightKeyIndex = i;
				}
			}
		}

		ASSERT( !(noLeftKey && noRightKey) ); // We can't have no keys at both left and right sides.

		if (noLeftKey)
		{
			// We're at the left of the first key
			return (*keysPtr)[rightKeyIndex].value;
		} 
		else if (noRightKey)
		{
			// We're at the right of the last key
			return (*keysPtr)[leftKeyIndex].value;
		} else 
		{
			const Pair& p1 = (*keysPtr)[leftKeyIndex];
			const Pair& p2 = (*keysPtr)[rightKeyIndex];

			if (interpolationType == InterpolationType_Tangents)
			{
				float timeRange = p2.time - p1.time;

				ASSERT( timeRange >= 0 ); // A logical result of our key search, but just in case.

				if (timeRange == 0)
				{
					return (p1.value + p2.value) * 0.5f;
				} 
				else 
				{
					float alpha = (time - p1.time) / timeRange; // 0..1

					float v1 = p1.value;
					float v2 = v1 + p1.outTangentY * timeRange;
					float v4 = p2.value;
					float v3 = v4 - p2.inTangentY * timeRange;

					// Cubic bezier curve, as used by Maya				

					float u = alpha;
					float uu = 1.0f-u;   // 1-u
					float uu2 = uu*uu;   // (1-u)^2
					float uu3 = uu*uu2;  // (1-u)^3

					// cubic bezier curve, used by Maya.
					return 
						uu3*v1 + 
						uu2*u*3.0f*v2 + 
						u*u*uu*3.0f*v3 + 
						u*u*u*v4;
				}
			}
			else
			{
				ASSERT( interpolationType == InterpolationType_Hold ); // sanity check

				// Always take left.
				return p1.value;
			}
		}
#endif
	}


	void AnimCurve::Tick(float deltaTime)
	{
		Element::Tick(deltaTime);

		// Update time and re-evaluate current value!
		currentTime += deltaTime;

		currentValue = GetValueAt(currentTime);
	}


	float AnimCurve::GetCurrentSegmentProgress() const
	{
		if (keysPtr->Size() == 0)
			return 0.0f;

		const Pair* finger = keysPtr->GetItemsPtr();
		float startTime = finger->time;

		for (int i=0; i<keysPtr->Size(); ++i, ++finger)
		{
			if (finger->time > currentTime)
			{
				float timeRange = finger->time - startTime;
				float alpha = (currentTime - startTime) / timeRange;
				if (alpha < 0)
					alpha = 0;
				else if (alpha > 1)
					alpha = 1;

				return alpha;
			}
			else
				startTime = finger->time;
		}

		return 1;
	}


#if PIMP_SUPPORT_DUMPANIMS
	void AnimCurve::RenderToFile(const char* filename, float duration, float timestep)
	{
		float backupCurrentTime = currentTime;
		float backupCurrentValue = currentValue;


		FILE* fff = NULL;
		fopen_s(&fff, filename, "wt");
		if (!fff)
			return;

		char s[512];
		sprintf_s(s, 512,
			"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
			"<!-- Created with Inkscape (http://www.inkscape.org/) -->\n"
			"<svg\n"
			"xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
			"xmlns=\"http://www.w3.org/2000/svg\"\n"
			"version=\"1.0\"\n"
			"width=\"210mm\"\n"
			"height=\"297mm\"\n"
			"id=\"svg2\">\n"
			"<defs\n"
			"id=\"defs5\" />\n"
			"<path\n"
			"d=\"\n");

		// header
		fwrite(s, 1, strlen(s), fff);

		const float hscale = 1000.0f;
		const float vscale = 30.0f;

		bool first = true;
		currentTime = 0;
		Tick(0);
		while (currentTime < duration)
		{
			if (first)
				sprintf_s(s,512, "M %f,%f ", currentTime*hscale, -currentValue*vscale);
			else
				sprintf_s(s,512, "L %f,%f ", currentTime*hscale, -currentValue*vscale);

			fwrite(s, 1, strlen(s), fff);
			
			first = false;
			Tick(timestep);
		}


		// footer
		sprintf_s(s, 512,
			"\n\"\n"
			"style=\"fill:none;fill-rule:evenodd;stroke:#000000;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
			"id=\"path2161\" />\n"
			"</svg>\n");
		fwrite(s, 1, strlen(s), fff);

		fclose(fff);

		currentTime = backupCurrentTime;
		currentValue = backupCurrentValue;
	}
#endif

}