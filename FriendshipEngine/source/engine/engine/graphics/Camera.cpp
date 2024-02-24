#include "pch.h"
#include "Camera.h"
#include "GraphicsEngine.h"

Camera::Camera() 
{
	myGraphicsEngine = GraphicsEngine::GetInstance();
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
	myProjection = {};

	myHFoV = aFov * PI / 180.f;
	myVFoV = 2 * std::atan(std::tan(myHFoV / 2) * (aHeight / aWidth));
	// Aspect ratio and scaling
	myProjection.r[0].m128_f32[0] = 1.f / tan(myHFoV / 2.f);
	myProjection.r[1].m128_f32[1] = 1.f / tan(myVFoV / 2.f);

	// Near-far plane
	myProjection.r[2].m128_f32[2] = aFarPlane / (aFarPlane - aNearPlane);
	myProjection.r[2].m128_f32[3] = 1.f / (aFarPlane / (aFarPlane - aNearPlane));
	myProjection.r[3].m128_f32[2] = -(aFarPlane / (aFarPlane - aNearPlane)) * aNearPlane;

	myProjection.r[3].m128_f32[3] = 0.f;
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

	myProjection.r[0].m128_f32[0] = 2.f / (aRight - aLeft);
	myProjection.r[1].m128_f32[1] = 2.f / (aBottom - aTop);
	myProjection.r[2].m128_f32[2] = 1.f / (aFar - aNear);

	myProjection.r[3].m128_f32[0] = -(aRight + aLeft) / (aRight - aLeft);
	myProjection.r[3].m128_f32[1] = -(aBottom + aTop) / (aBottom - aTop);
	myProjection.r[3].m128_f32[2] = -(aNear) / (aFar - aNear);
	myProjection.r[3].m128_f32[3] = 1;
	myProjection;
}

const Vector3f Camera::PointToPostProjection(const Vector3f& aPoint) const
{
	DirectX::XMFLOAT4 p;
	p.x = aPoint.x;
	p.y = aPoint.y;
	p.z = aPoint.z;
	p.w = 1.f;

	DirectX::XMVECTOR v = DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&p), GetViewMatrix());
	p = { v.m128_f32[0], v.m128_f32[1], v.m128_f32[2], v.m128_f32[3] };
	v = DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&p), GetModelMatrix());
	p = { v.m128_f32[0], v.m128_f32[1], v.m128_f32[2], v.m128_f32[3] };
	v = DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&p), GetProjectionMatrix());

	return { p.x / p.w, p.y / p.w, p.z / p.w };
}

const DirectX::XMMATRIX Camera::GetProjectionMatrix() const
{
	return myProjection;
}

const DirectX::XMMATRIX Camera::GetViewMatrix() const
{
	return myTransform.GetMatrix();
}

const DirectX::XMMATRIX Camera::GetModelMatrix() const
{
	return DirectX::XMMatrixInverse(nullptr, GetViewMatrix());
}

const Vector2<float> Camera::ProjectionToPixel(const Vector3<float>& aVec) const
{
	auto size = Vector2f(myGraphicsEngine->GetWindowDimensions());
	auto copy = PointToPostProjection(aVec);
	
	return Vector2<float>(copy.x * size.x / 2.f + size.x / 2.f, copy.y * size.y / 2.f + size.y / 2.f);
}
