#pragma once

#include "../math/Matrix.h"
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
	void SetPosition(const Vector3<float>& aPosition);
	void SetRotation(const Vector3<float>& aRotation);
	const Vector3<float> PointToPostProjection(const Vector3<float> &aPoint) const;
	const Matrix4x4<float> GetProjectionMatrix() const;
	const Matrix4x4<float> GetViewMatrix() const;
	const Matrix4x4<float> GetModelMatrix() const;
	const Vector3<float> GetForward() const;
	const Vector3<float> GetRight() const;
	const Vector3<float> GetUp() const;
	const Vector3<float> GetPosition() const { return myPosition; }
	const Vector2<float> ProjectionToPixel(const Vector3<float> &aVec) const;
	const bool IsInsideFrustum(Vector3<float> aPoint);

	Vector3<float>& GetRotation() { return myEulerRotation; }
	const Vector3<float>& GetRotation() const { return myEulerRotation; }

	const float& GetNearPlane() const { return myNearPlane; }
	const float& GetFarPlane() const { return myFarPlane; }
	const Vector2<float> GetNearPlaneDimensions() const { return { myWNear, myHNear }; }
	const Vector2<float> GetFarPlaneDimensions() const { return { myWFar, myHFar }; }
private:
	const float PI = 3.14159265f;
	Matrix4x4<float> myProjection;
	Vector3<float> myPosition;
	Vector3<float> myEulerRotation;
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
	Plane<float> myFrustum[6];

	GraphicsEngine* myGraphicsEngine;
};
