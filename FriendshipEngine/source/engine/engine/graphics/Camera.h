#pragma once

#include <directxtk/SimpleMath.h>
#include "../math/Transform.h"
#include "../math/Plane.h"

class GraphicsEngine;	

class Camera
{
public:
	Camera();
	void SetProjectionMatrix(
		const float& aFov,
		const float& aWidth,
		const float& aHeight,
		const float& aNearPlane,
		const float& aFarPlane
	);
	void SetOrtographicProjection(
		const float& aLeft,
		const float& aRight,
		const float& aTop,
		const float& aBottom,
		const float& aNear,
		const float& aFar
	);
	const Vector3f PointToPostProjection(const Vector3f &aPoint) const;
	const DirectX::XMMATRIX GetProjectionMatrix() const;
	const DirectX::XMMATRIX GetViewMatrix() const;
	const DirectX::XMMATRIX GetModelMatrix() const;
	const Vector2f ProjectionToPixel(const Vector3<float> &aVec) const;

	const float& GetNearPlane() const { return myNearPlane; }
	const float& GetFarPlane() const { return myFarPlane; }
	const Vector2<float> GetNearPlaneDimensions() const { return { myWNear, myHNear }; }
	const Vector2<float> GetFarPlaneDimensions() const { return { myWFar, myHFar }; }
	Transform& GetTransform() { return myTransform; }
private:
	const float PI = 3.14159265f;
	DirectX::XMMATRIX myProjection;
	Transform myTransform;
	float myFarPlane;
	float myNearPlane;
	float myHeight;
	float myWidth;
	float myHFar;
	float myHNear;
	float myWFar;
	float myWNear;
	float myVFoV;
	float myHFoV;

	GraphicsEngine* myGraphicsEngine;
};
