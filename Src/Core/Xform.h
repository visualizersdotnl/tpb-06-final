#pragma once

#include "Node.h"
#include "World.h"

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

		void CalculateTransforms();

	public:
		Xform(World* ownerWorld);
		Xform(const Xform& other);
		virtual ~Xform() {}

		virtual void Tick(float deltaTime);

		virtual const Matrix4* GetWorldTransform() const    { return &transformWorld;    }
		virtual const Matrix4* GetWorldTransformInv() const { return &transformWorldInv; }

		Matrix4& GetLocalTransform()
		{
			return transformLocal;
		}

		const Vector3& GetTranslation() const            { return translation; }
		const Quaternion& GetRotation() const            { return rotation; }
		const Quaternion& GetRotationOrientation() const { return rotationOrientation; }
		const Vector3& GetScale() const                  { return scale; }

		const Matrix4& GetRotationPivot() const            { return rotationPivot; }
		const Matrix4& GetRotationPivotInv() const         { return rotationPivotInv; }
		const Matrix4& GetRotationPivotTranslation() const { return rotationPivotTranslation; }

		const Matrix4& GetScalePivot() const            { return scalePivot; }
		const Matrix4& GetScalePivotInv() const         { return scalePivotInv; }
		const Matrix4& GetScalePivotTranslation() const { return scalePivotTranslation; }

		void SetTranslation(const Vector3& translation);
		void SetRotation(const Quaternion& rotation);
		void SetRotationOrientation(const Quaternion& rotationOrientation);
		void SetScale(const Vector3& scale);

		void SetRotationPivotTranslation(const Vector3& rotPivotTranslation);
		void SetRotationPivot(const Vector3& vecRotationPivot);
		void SetScalePivotTranslation(const Vector3& vecScalePivotTrans);
		void SetScalePivot(const Vector3& vecScalePivot);
	};
}
