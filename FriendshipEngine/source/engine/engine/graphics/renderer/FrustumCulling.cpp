#include "pch.h"
#include "FrustumCulling.h"
#include "../Camera.h"
#include "../DxData.h"
#include "../GraphicsEngine.h"





void FrustumCulling::SetFarAndNearPlanes()
{
	auto cam = GraphicsEngine::GetInstance()->GetCamera();
	auto camPos = cam->GetTransform().GetPosition();
	auto camUp = cam->GetTransform().GetUp();
	auto camForward = cam->GetTransform().GetForward();
	auto camRight = cam->GetTransform().GetRight();
	auto hFar = cam->GetHFar();
	auto wFar = cam->GetVFar();
	auto hNear = cam->GetHNear();
	auto wNear = cam->GetVNear();

	Vector3<float> farCenter = camPos + (camForward * cam->GetFarPlane());

	myFarTopLeft = farCenter + (camUp * hFar / 2.f) - (camRight * wFar / 2.f);
	myFarTopRight = farCenter + (camUp * hFar / 2.f) + (camRight * wFar / 2.f);
	myFarBottomLeft = farCenter - (camUp * hFar / 2.f) - (camRight * wFar / 2.f);
	myFarBottomRight = farCenter - (camUp * hFar / 2.f) + (camRight * wFar / 2.f);

	Vector3<float> nearCenter = camPos + (camForward * cam->GetNearPlane());

	myNearTopLeft = nearCenter + (camUp * hNear / 2.f) - (camRight * wNear / 2.f);
	myNearTopRight = nearCenter + (camUp * hNear / 2.f) + (camRight * wNear / 2.f);
	myNearBottomLeft = nearCenter - (camUp * hNear / 2.f) - (camRight * wNear / 2.f);
	myNearBottomRight = nearCenter - (camUp * hNear / 2.f) + (camRight * wNear / 2.f);



	Vector3f point;
	Vector3f aux;
	Vector3f normal;

	myFrustum.nearFace.myNormal = camForward;
	myFrustum.nearFace.myPoint = nearCenter;

	myFrustum.nearFace.myNormal = -1.f * camForward;
	myFrustum.nearFace.myPoint = farCenter;

	point = (nearCenter + camUp * (hNear / 2.f));
	aux = (point - camPos).GetNormalized();
	normal = Vector3f::Cross(camRight, aux);
	myFrustum.topFace.myNormal = normal;
	myFrustum.topFace.myPoint = point;

	point = (nearCenter - camUp * (hNear / 2.f));
	aux = (point - camPos).GetNormalized();;
	normal = Vector3f::Cross(aux, camRight);
	myFrustum.bottomFace.myNormal = normal;
	myFrustum.bottomFace.myPoint = point;


	point = (nearCenter - camRight * wNear / 2.f);
	aux = (point - camPos).GetNormalized();;
	normal = Vector3f::Cross(camUp, aux);
	myFrustum.leftFace.myNormal = normal;
	myFrustum.leftFace.myPoint = point;


	point = (nearCenter + camRight * (wNear / 2.f));
	aux = (point - camPos).GetNormalized();
	normal = Vector3f::Cross(aux, camUp);
	myFrustum.rightFace.myNormal = normal;
	myFrustum.rightFace.myPoint = point;

	//skip = 0;
	//myLines.clear();
	//line1.SetStartAndEnd(camPos, myFarTopLeft);
	//line1.SetColor({ 1,1,1,1 });
	//line2.SetStartAndEnd(camPos, myFarTopRight);
	//line2.SetColor({ 1,1,1,1 });
	//line3.SetStartAndEnd(camPos, myFarBottomLeft);
	//line3.SetStartAndEnd(camPos, myFarBottomLeft);
	//line3.SetColor({ 1,1,1,1 });
	//line4.SetStartAndEnd(camPos, myFarBottomRight);
	//line4.SetColor({ 1,1,1,1 });

	//line1.DrawLine();
	//	line2.DrawLine();
	//	line3.DrawLine();
	//	line4.DrawLine();
}

void FrustumCulling::Initialize(Camera& aCam)
{
	aCam;

}

void FrustumCulling::DRAWALLTHEMLINES()
{
	for (int i = 0; i < myLines.size(); i++)
	{
		myLines[i].DrawLine();
	}
}
void FrustumCulling::LOADALLTTHEMLINES(Vector3f aPos, float aRadius)
{
	myLines.push_back(DebugLine({ aPos.x - aRadius,aPos.y,aPos.z }, { aPos.x + aRadius,aPos.y,aPos.z }, { 1,1,1,1 }));

	myLines.push_back(DebugLine({ aPos.x,aPos.y - aRadius,aPos.z }, { aPos.x,aPos.y + aRadius,aPos.z }, { 1,1,1,1 }));

	myLines.push_back(DebugLine({ aPos.x,aPos.y ,aPos.z - aRadius }, { aPos.x,aPos.y,aPos.z + aRadius }, { 1,1,1,1 }));

}

bool FrustumCulling::CullMesh(const BoundingCullSphere& aSphere) //Entity& aEntity
{




	if (IsOnForwardPlane(myFrustum.leftFace, aSphere) &&
		IsOnForwardPlane(myFrustum.rightFace, aSphere) &&
		IsOnForwardPlane(myFrustum.topFace, aSphere) &&
		IsOnForwardPlane(myFrustum.bottomFace, aSphere) &&
		IsOnForwardPlane(myFrustum.farFace, aSphere) &&
		IsOnForwardPlane(myFrustum.nearFace, aSphere))
							return false;

	return true;
}


bool FrustumCulling::IsOnForwardPlane(FrustumPlane plane,const BoundingCullSphere& sphere)
{
	float distance = Vector3<float>::Dot(plane.myNormal, (sphere.myCenter - plane.myPoint));

	if (distance < -sphere.myRadius)
	{
		return false;
	}

	return true;
}

bool FrustumCulling::IsInsideOfFrustum()
{
	return false;
}

float FrustumCulling::GetDistanceToPlane(Vector3<float> aPoint, FrustumPlane aPlane)
{
	aPoint;
	aPlane;
	return 1.f;
}
