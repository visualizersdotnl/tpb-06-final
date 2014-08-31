#pragma once

#include "Node.h"
#include "World.h"
#include "AnimCurve.h"

namespace Pimp
{
	class Xform 
		: public Node
	{
	private:
		Matrix4 transformLocal;
		Matrix4 transformWorld;
		Matrix4 transformWorldInv;

		Vector3 translation;
		Quaternion rotation;
		Quaternion rotationOrientation;
		Vector3 scale;

		Matrix4 rotationPivot;
		Matrix4 rotationPivotInv;
		Matrix4 rotationPivotTranslation;

		Matrix4 scalePivot;
		Matrix4 scalePivotInv;
		Matrix4 scalePivotTranslation; 

		AnimCurve* animCurveTranslation[3];
		AnimCurve* animCurveRotation[3];
		AnimCurve* animCurveScale[3];

		bool hasAnimsAtAll;

		void CalculateTransforms();
		void HandledAnimation();

		void UpdateHasAnims();

#ifdef _DEBUG
		static bool allowReCalculateTransformsOnTick;
#endif

	public:
		Xform(World* ownerWorld);
		Xform(const Xform& other);
#ifdef _DEBUG
		virtual ~Xform();
#endif

		virtual void Tick(float deltaTime);

		virtual const Matrix4* GetWorldTransform() const
		{
			return &transformWorld;
		}

		virtual const Matrix4* GetWorldTransformInv() const
		{
			return &transformWorldInv;
		}

		Matrix4& GetLocalTransform()
		{
			return transformLocal;
		}

		const Vector3& GetTranslation() const 
		{
			return translation;
		}

		AnimCurve* GetAnimCurveRotationX() const { return animCurveRotation[0]; }
		AnimCurve* GetAnimCurveRotationY() const { return animCurveRotation[1]; }
		AnimCurve* GetAnimCurveRotationZ() const { return animCurveRotation[2]; }
		AnimCurve* GetAnimCurveTranslationX() const { return animCurveTranslation[0]; }
		AnimCurve* GetAnimCurveTranslationY() const { return animCurveTranslation[1]; }
		AnimCurve* GetAnimCurveTranslationZ() const { return animCurveTranslation[2]; }

		void SetTranslation(const Vector3& translation);
		void SetRotation(const Quaternion& rotation);
		void SetRotationOrientation(const Quaternion& rotationOrientation);
		void SetScale(const Vector3& scale);

		void SetRotationPivotTranslation(const Vector3& rotPivotTranslation);
		void SetRotationPivot(const Vector3& vecRotationPivot);
		void SetScalePivotTranslation(const Vector3& vecScalePivotTrans);
		void SetScalePivot(const Vector3& vecScalePivot);

		void SetAnimCurveTranslationX(AnimCurve* curve);
		void SetAnimCurveTranslationY(AnimCurve* curve);
		void SetAnimCurveTranslationZ(AnimCurve* curve);

		void SetAnimCurveRotationX(AnimCurve* curve);
		void SetAnimCurveRotationY(AnimCurve* curve);
		void SetAnimCurveRotationZ(AnimCurve* curve);

		void SetAnimCurveScaleX(AnimCurve* curve);
		void SetAnimCurveScaleY(AnimCurve* curve);
		void SetAnimCurveScaleZ(AnimCurve* curve);

#ifdef _DEBUG
		static void SetAllowReCalculateTransformsOnTick(bool v) { allowReCalculateTransformsOnTick = v; }
#endif
	};
}