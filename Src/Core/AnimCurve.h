#pragma once

#include "Element.h"
#include <Shared/shared.h>
#include "Settings.h"

namespace Pimp
{
	class AnimCurve : public Element
	{
	public:
		struct Pair
		{
			float time;
			float value;

			float inTangentY;
			float outTangentY;
		};

		enum InterpolationType
		{
			InterpolationType_Tangents,
			InterpolationType_Hold
		};

	private:
		FixedSizeList<Pair>* keysPtr;
		float currentTime;
		float currentValue;

		InterpolationType interpolationType;

	public:
		AnimCurve(World* ownerWorld);
		~AnimCurve();

		FixedSizeList<Pair>* GetKeysPtr() const 
		{
			return keysPtr;
		}

		void SetKeysPtr(FixedSizeList<Pair>* keysPtr)
		{
			this->keysPtr = keysPtr;
		}

		void SetKeysPtr(const Pair* pairs, int numPairs);

		void ForceSetTime(float time);

		virtual void Tick(float deltaTime);

		float GetCurrentValue() const 
		{
			return currentValue;
		}

		float GetCurrentSegmentProgress() const;

		float GetValueAt(float time) const;

		bool HasKeys() const
		{
			return keysPtr != NULL && keysPtr->Size() > 0;
		}

		InterpolationType GetInterpolationType() const
		{
			return interpolationType;
		}

		void SetInterpolationType(InterpolationType newInterpolationType)
		{
			interpolationType = newInterpolationType;
		}


#if PIMP_SUPPORT_DUMPANIMS
		void RenderToFile(const char* filename, float duration, float timestep);
#endif
	};
}