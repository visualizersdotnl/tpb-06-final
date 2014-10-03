#include "Settings.h"
#include "Xform.h"

namespace Pimp 
{
#ifdef _DEBUG
	bool Xform::allowReCalculateTransformsOnTick = true;
#endif

	Xform::Xform(Pimp::World *ownerWorld)
		: Node(ownerWorld),
		translation(0,0,0),
		rotation(0,0,0,1),
		rotationOrientation(0,0,0,1),
		scale(1,1,1),
		rotationPivot(Matrix4::GetIdentity()),
		rotationPivotInv(Matrix4::GetIdentity()),
		rotationPivotTranslation(Matrix4::GetIdentity()),
		scalePivot(Matrix4::GetIdentity()),
		scalePivotInv(Matrix4::GetIdentity()),
		scalePivotTranslation(Matrix4::GetIdentity()),
		hasAnimsAtAll(true)
	{
		for (int i=0; i<3; ++i)
		{
			animCurveTranslation[i] = NULL;
			animCurveRotation[i] = NULL;
			animCurveScale[i] = NULL;
		}

		SetType(ET_Xform); 
		CalculateTransforms();
	}

	Xform::Xform(const Xform& other)
		: Node(other.ownerWorld),
		translation(other.translation),
		rotation(other.rotation),
		rotationOrientation(other.rotationOrientation),
		scale(other.scale), rotationPivot(other.rotationPivot),
		rotationPivotInv(other.rotationPivotInv), rotationPivotTranslation(other.rotationPivotTranslation),
		scalePivot(other.scalePivot), scalePivotInv(other.scalePivotInv), scalePivotTranslation(other.scalePivotTranslation),
		hasAnimsAtAll(other.hasAnimsAtAll)
	{
		for (int i=0; i<3; ++i)
		{
			animCurveTranslation[i] = other.animCurveTranslation[i];
			animCurveRotation[i] = other.animCurveRotation[i];
			animCurveScale[i] = other.animCurveScale[i];
		}

		SetType(ET_Xform); 
		CalculateTransforms();
	}

	void Xform::CalculateTransforms()
	{
		// Build transformation matrix.  NULL arguments are treated as identity.

		//    -1                      -1
		// [Sp]x[S]x[Sh]x[Sp]x[St]x[Rp]x[Ro]x[R]x[Rp]x[Rt]x[T]
		// 
		// M = Msp-1 * Ms * Msp * Mst * Mrp-1 * Mro * Mr * Mrp * Mrt * Mt
		// 
		// Msp = scale pivot = TranslationMat(scalepivot)
		// Ms = scale matrix = Identity * scalevec
		// Mst = scale pivot translation = TranslationMat(scalepivottranslation)
		// Mrp = rotate pivot point = TranslationMat(rotatepivot)
		// Mro = rotation orientation = matrix
		// Mr = rotation matrix
		// Mrt = rotation pivot translation = TranslationMat(rotationpivottranslation)
		// 


		Matrix4 r,s,t,ro;

		r = CreateMatrixRotationQuaternion(rotation);
		ro = CreateMatrixRotationQuaternion(rotationOrientation);
		
		t = CreateMatrixTranslation(translation);
		s = CreateMatrixScaling(scale);

		transformLocal = scalePivotInv * s;
		transformLocal *= scalePivot;
		transformLocal *= scalePivotTranslation;

		transformLocal *= rotationPivotInv;
		transformLocal *= ro;
		transformLocal *= r;
		transformLocal *= rotationPivot;
		transformLocal *= rotationPivotTranslation;
		transformLocal *= t;
	}



	void Xform::HandledAnimation()
	{
#ifdef _DEBUG
		if (!allowReCalculateTransformsOnTick)
			return;
#endif

		bool needsUpdateTransform = false;

		//
		// Translation
		//

		if (animCurveTranslation[0] != NULL)
		{
			translation.x = animCurveTranslation[0]->GetCurrentValue();
			needsUpdateTransform = true;
		}
		if (animCurveTranslation[1] != NULL)
		{
			translation.y = animCurveTranslation[1]->GetCurrentValue();
			needsUpdateTransform = true;
		}
		if (animCurveTranslation[2] != NULL)
		{
			translation.z = animCurveTranslation[2]->GetCurrentValue();
			needsUpdateTransform = true;
		}

		//
		// Rotation (only allow rotation over all axes simultaneously)
		//

		if (animCurveRotation[0] && animCurveRotation[1] && animCurveRotation[2])
		{
			Matrix4 x,y,z, res;

			x = CreateMatrixRotationX(animCurveRotation[0]->GetCurrentValue());
			y = CreateMatrixRotationY(animCurveRotation[1]->GetCurrentValue());
			z = CreateMatrixRotationZ(animCurveRotation[2]->GetCurrentValue());

			res = (x*y) * z;

			Quaternion q = CreateQuaternionFromRotationMatrix(res);

			rotation.x = q.x;
			rotation.y = q.y;
			rotation.z = q.z;
			rotation.w = q.w;	
			needsUpdateTransform = true;
		}

		//
		// Scale
		//
		
		if (animCurveScale[0] != NULL)
		{
			scale.x = animCurveScale[0]->GetCurrentValue();
			needsUpdateTransform = true;
		}
		if (animCurveScale[1] != NULL)
		{
			scale.y = animCurveScale[1]->GetCurrentValue();
			needsUpdateTransform = true;
		}
		if (animCurveScale[2] != NULL)
		{
			scale.z = animCurveScale[2]->GetCurrentValue();
			needsUpdateTransform = true;
		}


		if (needsUpdateTransform)
			CalculateTransforms();
	}


	void Xform::Tick(float deltaTime)
	{
		if (hasAnimsAtAll)
		{
			HandledAnimation();
		}	

		// Calc new world
		const Matrix4* world = (parents.Size() > 0) ? parents[0]->GetWorldTransform() : Matrix4::GetIdentityPtr();
		
		transformWorld = transformLocal * (*world);
		transformWorldInv = transformWorld.Inversed();

		// Tick children 
		Node::Tick(deltaTime);
	}


	void Xform::SetTranslation(const Vector3& translation)
	{
		this->translation = translation;
		CalculateTransforms();
	}

	void Xform::SetRotation(const Quaternion& rotation)
	{
		this->rotation = rotation;
		CalculateTransforms();
	}

	void Xform::SetRotationOrientation(const Quaternion& rotationOrientation)
	{
		this->rotationOrientation = rotationOrientation;
		CalculateTransforms();
	}

	void Xform::SetScale(const Vector3& scale)
	{
		this->scale = scale;
		CalculateTransforms();
	}

	void Xform::SetRotationPivotTranslation(const Vector3& rotPivotTranslation)
	{
		rotationPivotTranslation = CreateMatrixTranslation(rotPivotTranslation);

		CalculateTransforms();
	}

	void Xform::SetRotationPivot(const Vector3& vecRotationPivot)
	{
		rotationPivot = CreateMatrixTranslation(vecRotationPivot);
		rotationPivotInv = rotationPivot.Inversed();

		CalculateTransforms();
	}

	void Xform::SetScalePivotTranslation(const Vector3& vecScalePivotTrans)
	{
		scalePivotTranslation = CreateMatrixTranslation(vecScalePivotTrans);

		CalculateTransforms();
	}

	void Xform::SetScalePivot(const Vector3& vecScalePivot)
	{
		scalePivot = CreateMatrixTranslation(vecScalePivot);
		scalePivotInv = scalePivot.Inversed();

		CalculateTransforms();
	}




	void Xform::SetAnimCurveTranslationX(AnimCurve* curve)
	{
#ifndef _DEBUG
		if (curve != NULL && !curve->HasKeys())
			curve = NULL;
#endif

		animCurveTranslation[0] = curve;
		UpdateHasAnims();
	}

	void Xform::SetAnimCurveTranslationY(AnimCurve* curve)
	{
#ifndef _DEBUG
		if (curve != NULL && !curve->HasKeys())
			curve = NULL;
#endif

		animCurveTranslation[1] = curve;
		UpdateHasAnims();
	}

	void Xform::SetAnimCurveTranslationZ(AnimCurve* curve)
	{
#ifndef _DEBUG
		if (curve != NULL && !curve->HasKeys())
			curve = NULL;
#endif

		animCurveTranslation[2] = curve;
		UpdateHasAnims();
	}

	void Xform::SetAnimCurveRotationX(AnimCurve* curve)
	{
#ifndef _DEBUG
		if (curve != NULL && !curve->HasKeys())
			curve = NULL;
#endif

		animCurveRotation[0] = curve;
		UpdateHasAnims();
	}

	void Xform::SetAnimCurveRotationY(AnimCurve* curve)
	{
#ifndef _DEBUG
		if (curve != NULL && !curve->HasKeys())
			curve = NULL;
#endif

		animCurveRotation[1] = curve;
		UpdateHasAnims();
	}

	void Xform::SetAnimCurveRotationZ(AnimCurve* curve)
	{
#ifndef _DEBUG
		if (curve != NULL && !curve->HasKeys())
			curve = NULL;
#endif

		animCurveRotation[2] = curve;
		UpdateHasAnims();
	}

	void Xform::SetAnimCurveScaleX(AnimCurve* curve)
	{
#ifndef _DEBUG
		if (curve != NULL && !curve->HasKeys())
			curve = NULL;
#endif

		animCurveScale[0] = curve;
		UpdateHasAnims();
	}

	void Xform::SetAnimCurveScaleY(AnimCurve* curve)
	{
#ifndef _DEBUG
		if (curve != NULL && !curve->HasKeys())
			curve = NULL;
#endif

		animCurveScale[1] = curve;
		UpdateHasAnims();
	}

	void Xform::SetAnimCurveScaleZ(AnimCurve* curve)
	{
#ifndef _DEBUG
		if (curve != NULL && !curve->HasKeys())
			curve = NULL;
#endif

		animCurveScale[2] = curve;
		UpdateHasAnims();
	}

	void Xform::UpdateHasAnims()
	{
		hasAnimsAtAll = true; // Pessimistic guess

		for (int i=0; i<3; ++i)
		{
			if (animCurveTranslation[i] || animCurveRotation[i] || animCurveScale[i])
				return;
		}

		hasAnimsAtAll = false; // Apparently not!
	}
}