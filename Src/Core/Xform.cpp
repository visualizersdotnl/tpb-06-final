
#include "Platform.h"
#include "D3D.h"
#include "Xform.h"

namespace Pimp 
{
	Xform::Xform(Pimp::World *ownerWorld)
		: Node(ownerWorld)
,		translation(0,0,0)
,		rotation(0,0,0,1)
,		rotationOrientation(0,0,0,1)
,		scale(1,1,1)
,		rotationPivot(Matrix4::GetIdentity())
,		rotationPivotInv(Matrix4::GetIdentity())
,		rotationPivotTranslation(Matrix4::GetIdentity())
,		scalePivot(Matrix4::GetIdentity())
,		scalePivotInv(Matrix4::GetIdentity())
,		scalePivotTranslation(Matrix4::GetIdentity())
	{
		SetType(ET_Xform); 
		CalculateTransforms();
	}

	Xform::Xform(const Xform& other)
		: Node(other.ownerWorld)
,		translation(other.translation)
,		rotation(other.rotation)
,		rotationOrientation(other.rotationOrientation)
,		scale(other.scale), rotationPivot(other.rotationPivot)
,		rotationPivotInv(other.rotationPivotInv), rotationPivotTranslation(other.rotationPivotTranslation)
,		scalePivot(other.scalePivot), scalePivotInv(other.scalePivotInv), scalePivotTranslation(other.scalePivotTranslation)
	{
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

	void Xform::Tick(float deltaTime)
	{
		// Update world matrix
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
}
