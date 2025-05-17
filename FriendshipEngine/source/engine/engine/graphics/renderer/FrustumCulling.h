#pragma once
#include <engine/math/Vector3.h>
#include <ecs/entity/Entity.h>
#include<vector>

#include <engine/debug/DebugLine.h>


class Camera;

struct FrustumPlane
{
	FrustumPlane() {};
	FrustumPlane(Vector3<float> p1, Vector3<float> aNormal)
	{
		myNormal = aNormal.GetNormalized();;
		myPoint = p1;
	}


	Vector3<float> myNormal;
	Vector3<float> myPoint;
};

struct BoundingCullSphere
{
	BoundingCullSphere(Vector3<float> aCenter, float aRadius)
	{
		myCenter = aCenter;
		myRadius = aRadius;
	}
	Vector3<float> myCenter;
	float myRadius;
};

struct Frustum
{
	FrustumPlane nearFace;
	FrustumPlane farFace;
	FrustumPlane leftFace;
	FrustumPlane rightFace;
	FrustumPlane topFace;
	FrustumPlane bottomFace;
};


class FrustumCulling
{
public:
	FrustumCulling() = default;

	void SetFarAndNearPlanes();
	void Initialize(Camera& aCam);
	bool CullMesh(const BoundingCullSphere& aSphere);//Entity& aEntity

	void LOADALLTTHEMLINES(Vector3f aPos, float aRadius);
	void DRAWALLTHEMLINES();

private:
	bool IsOnForwardPlane(FrustumPlane plane, const BoundingCullSphere& sphere);
	bool IsInsideOfFrustum();
	float GetDistanceToPlane(Vector3<float> aPoint, FrustumPlane aPlane);



	Vector3<float> myFarTopRight;
	Vector3<float> myFarTopLeft;
	Vector3<float> myFarBottomLeft;
	Vector3<float> myFarBottomRight;

	Vector3<float> myNearTopRight;
	Vector3<float> myNearTopLeft;
	Vector3<float> myNearBottomLeft;
	Vector3<float> myNearBottomRight;

	Frustum myFrustum;



	DebugLine line1;
	DebugLine line2;
	DebugLine line3;
	DebugLine line4;
	int skip = 0;
	std::vector<DebugLine> myLines;
};