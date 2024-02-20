#include "pch.h"
#include "Camera.h"
#include "GraphicsEngine.h"

Camera::Camera() 
{
	myGraphicsEngine = GraphicsEngine::GetInstance();
	myPosition = Vector3<float>(0.f, 10.f, 0.f);
	myEulerRotation = Vector3<float>(0.f, 0.f, 0.f);
	myFarPlane = 0.f;
	myNearPlane = 0.f;
	myHeight = 0.f;
	myWidth = 0.f;
	myHFar = 0.f;
	myHNear = 0.f;
	myWFar = 0.f;
	myWNear = 0.f;
	myVFoV = 0.f;
	myHFoV = 0.f;
}

void Camera::SetProjectionMatrix(const float& aFov, const float& aWidth, const float& aHeight, const float& aNearPlane, const float& aFarPlane)
{
	myProjection = Matrix4x4<float>();

	myHFoV = aFov * PI / 180.f;
	myVFoV = 2 * std::atan(std::tan(myHFoV / 2) * (aHeight / aWidth));
	// Aspect ratio and scaling
	myProjection(1, 1) = 1.f / tan(myHFoV / 2.f);
	myProjection(2, 2) = 1.f / tan(myVFoV / 2.f);

	// Near-far plane
	myProjection(3, 3) = aFarPlane / (aFarPlane - aNearPlane);
	myProjection(3, 4) = 1.f / (aFarPlane / (aFarPlane - aNearPlane));
	myProjection(4, 3) = -(aFarPlane / (aFarPlane - aNearPlane)) * aNearPlane;

	myProjection(4, 4) = 0.f;
	myFarPlane = aFarPlane;
	myNearPlane = aNearPlane;
	myHeight = aHeight;
	myWidth = aWidth;
	myHFar = 2 * tan(myHFoV / 2.f) * aFarPlane;
	myWFar = myHFar * myHFoV / myVFoV;
	myHNear = 2 * tan(myHFoV / 2.f) * aNearPlane;
	myWNear = myHNear * myHFoV / myVFoV;
}

void Camera::SetOrtographicProjection(const float& aLeft, const float& aRight, const float& aTop, const float& aBottom, const float& aNear, const float& aFar)
{
	myProjection = {};

	myProjection(1, 1) = 2.f / (aRight - aLeft);
	myProjection(2, 2) = 2.f / (aBottom - aTop);
	myProjection(3, 3) = 1.f / (aFar - aNear);

	myProjection(4, 1) = -(aRight + aLeft) / (aRight - aLeft);
	myProjection(4, 2) = -(aBottom + aTop) / (aBottom - aTop);
	myProjection(4, 3) = -(aNear) / (aFar - aNear);
}

void Camera::SetPosition(const Vector3<float>& aPosition)
{
	myPosition = aPosition;
}

void Camera::SetRotation(const Vector3<float>& aRotation)
{
	myEulerRotation = aRotation;
}

const Vector3<float> Camera::PointToPostProjection(const Vector3<float>& aPoint) const
{
	Vector4<float> p;
	p.x = aPoint.x;
	p.y = aPoint.y;
	p.z = aPoint.z;
	p.w = 1.f;

	p = p * GetModelMatrix();
	p = p * GetProjectionMatrix();

	return { p.x / p.w, p.y / p.w, p.z / p.w };
}

const Matrix4x4<float> Camera::GetProjectionMatrix() const
{
	return myProjection;
}

const Matrix4x4<float> Camera::GetViewMatrix() const
{
	Matrix4x4<float> result = Matrix4x4<float>::CreateRotationAroundX(myEulerRotation.x * PI / 180.f) *
		Matrix4x4<float>::CreateRotationAroundY(myEulerRotation.y * PI / 180.f) * 
		Matrix4x4<float>::CreateRotationAroundZ(myEulerRotation.z * PI / 180.f);

	result *= Matrix4x4<float>::CreateTranslationMatrix(myPosition);

	//result = Matrix4x4<float>::CreateRollPitchYawMatrix(myEulerRotation.x, myEulerRotation.y, myEulerRotation.z);
	//result *= Matrix4x4<float>::CreateTranslationMatrix(myPosition.x, myPosition.y, myPosition.z);

	return result;
}

const Matrix4x4<float> Camera::GetModelMatrix() const
{
	return Matrix4x4<float>::GetFastInverse(GetViewMatrix());
}

const Vector3<float> Camera::GetForward() const
{
	Matrix4x4<float> copy = GetViewMatrix();
	return { copy(3, 1), copy(3, 2), copy(3, 3) };
}

const Vector3<float> Camera::GetRight() const
{
	Matrix4x4<float> copy = GetViewMatrix();
	return { copy(1, 1), copy(1, 2), copy(1, 3) };
}

const Vector3<float> Camera::GetUp() const
{
	Matrix4x4<float> copy = GetViewMatrix();
	return { copy(2, 1), copy(2, 2), copy(2, 3) };
}

const Vector2<float> Camera::ProjectionToPixel(const Vector3<float>& aVec) const
{
	auto size = Vector2<float>(myGraphicsEngine->GetWindowDimensions());
	auto copy = PointToPostProjection(aVec);
	
	return Vector2<float>(copy.x * size.x / 2.f + size.x / 2.f, copy.y * size.y / 2.f + size.y / 2.f);
}

const bool Camera::IsInsideFrustum(Vector3<float> aPoint)
{
	Vector4<float> p;
	Vector3<float> pP;
	p.x = aPoint.x;
	p.y = aPoint.y;
	p.z = aPoint.z;
	p.w = 1.f;
	p = p * GetModelMatrix();
	pP = { p.x, p.y, p.z };

	Vector3<float> forward = { 0, 0, 1.f };
	if (pP.z >= (forward * myFarPlane).z || pP.z <= (forward * myNearPlane).z)
	{
		return false;
	}

	Vector3<float> aP = PointToPostProjection(aPoint);

	Vector3<float> up = { 0, 1.f, 0 };
	Vector3<float> right = { 1.f, 0, 0 };

	Vector3<float> fc = forward * aP.z;
	Vector3<float> fbr = fc - up + right;

	if (aP.x > fbr.x || aP.y < fbr.y) return false;

	Vector3<float> ftl = fc + up - right;

	return aP.x >= ftl.x && aP.y <= ftl.y;
}
