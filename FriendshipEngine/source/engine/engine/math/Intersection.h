#pragma once
#include "Vector.h"
#include "Line.h"
#include "Plane.h"
#include "Sphere.h"
#include "Ray.h"
#include "AABB3D.h"
#include <cmath>
#include "Matrix.h"

// If the ray does not intersect the plane aOutIntersectionPoint remains unchanged and false is returned.
// If the ray intersects the plane the intersection point is stored in aOutIntersectionPoint and true is returned.
template<class T>
inline bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
{
	//Vector3<T> n = aPlane.GetNormal();
	//Vector3<T> p = aRay.GetOrigin();


	//T i = Vector3<T>(aPlane.GetPoint() - p).Dot(n);
	//if (i > -0.000001f && i < 0.000001f)
	//{
	//	aOutIntersectionPoint = aPlane.GetPoint();
	//	return true; // is inside plane
	//}



	//T dirdn = aRay.GetDirection().Dot(aPlane.GetNormal());

	//if (dirdn == 0)
	//{
	//	return false;
	//}

	//T d = aPlane.GetPoint().Dot(aPlane.GetNormal());
	//T p0n = aRay.GetOrigin().Dot(aPlane.GetNormal());
	//T distance = (d - p0n) / dirdn;
	////T distance = aPlane.GetPoint().Dot(aPlane.GetNormal()) - (aRay.GetOrigin().Dot(aPlane.GetNormal())) / aRay.GetDirection().Dot(aPlane.GetNormal());

	//if (distance < 0)
	//{
	//	return false;
	//}

	//aOutIntersectionPoint = aRay.GetOrigin() + aRay.GetDirection() * distance;
	//return true;

	Vector3<T> n = aPlane.GetNormal();
	Vector3<T> p = aRay.GetOrigin();

	T i = Vector3<T>(aPlane.GetPoint() - p).Dot(n);
	if (i > -0.000001f && i < 0.000001f)
	{
		aOutIntersectionPoint = aPlane.GetPoint();
		return true; // is inside plane
	}

	if (aRay.GetDirection().Dot(aPlane.GetNormal()) == 0.f)
	{
		return false;
	}

	T distance = (aPlane.GetPoint().Dot(aPlane.GetNormal()) - aRay.GetOrigin().Dot(aPlane.GetNormal())) / aRay.GetDirection().Dot(aPlane.GetNormal());

	if (distance < 0)
	{
		return false;
	}

	aOutIntersectionPoint = aRay.GetOrigin() + aRay.GetDirection() * distance;
	return true;

}

template<typename T>
inline bool IsInside(const T& aRayOriginValue, const T& aMinValue, const T& aMaxValue, T& anOutValue)
{
	T delta = aMaxValue - aMinValue;
	if (aRayOriginValue < aMinValue)
	{
		anOutValue = aMinValue - aRayOriginValue;
		if (anOutValue > delta) return false;
	}
	else if (aRayOriginValue > aMaxValue)
	{
		anOutValue = aMaxValue - aRayOriginValue;
		if (anOutValue < delta) return false;
	}
	else
	{
		anOutValue = 1.0f;
	}
	return true;
}

template<typename T>
inline bool IntersectionAABBRay(const AABB3D<T>& aAABB, const Vector3<T>& aAABBPosition, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
{
	aAABBPosition;

	T noIntersection = FLT_MAX;

	bool isInside = true;

	auto origin = aRay.GetOrigin();
	auto direction = aRay.GetDirection();

	auto minPoint = aAABB.GetMin() + aAABBPosition;
	auto maxPoint = aAABB.GetMax() + aAABBPosition;

	//auto minPoint = aAABB.GetMin();
	//auto maxPoint = aAABB.GetMax();

	float xt = 0.0f;
	float xn = 0.0f;
	if (origin.x < minPoint.x)
	{
		xt = minPoint.x - origin.x;
		if (xt > direction.x) { return false; }
		xt /= direction.x;
		isInside = false;
		xn = -1.0f;
	}
	else if (origin.x > maxPoint.x)
	{
		xt = maxPoint.x - origin.x;
		if (xt < direction.x) { return false; }

		xt /= direction.x;
		isInside = false;
		xn = 1.0f;
	}
	else
	{
		xt = -1.0f;
	}

	float yt = 0.0f;
	float yn = 0.0f;
	if (origin.y < minPoint.y)
	{
		yt = minPoint.y - origin.y;

		if (yt > direction.y) { return false; }

		yt /= direction.y;
		isInside = false;
		yn = -1.0f;
	}
	else if (origin.y > maxPoint.y)
	{
		yt = maxPoint.y - origin.y;
		if (yt < direction.y) { return false; }
		yt /= direction.y;
		isInside = false;
		yn = 1.0f;
	}
	else
	{
		yt = -1.0f;
	}

	float zt = 0.0f;
	float zn = 0.0f;
	if (origin.z < minPoint.z)
	{
		zt = minPoint.z - origin.z;
		if (zt > direction.z)
		{
			aOutIntersectionPoint = origin + direction * noIntersection;
			return false;
		}
		zt /= direction.z;
		isInside = false;
		zn = -1.0f;
	}
	else if (origin.z > maxPoint.z)
	{
		zt = maxPoint.z - origin.z;
		if (zt < direction.z) { return false; }
		zt /= direction.z;
		isInside = false;
		zn = 1.0f;
	}
	else
	{
		zt = -1.0f;
	}

	if (isInside)
	{
		//aOutIntersectionPoint.x = -direction.x;
		//aOutIntersectionPoint.y = -direction.y;
		//aOutIntersectionPoint.z = -direction.z;
		//aOutIntersectionPoint.Normalize();
		aOutIntersectionPoint = origin;
		return true;
	}
	int which = 0;
	float t = xt;
	if (yt > t)
	{
		which = 1;
		t = yt;
	}
	if (zt > t)
	{
		which = 2;
		t = zt;
	}

	switch (which)
	{
	case 0:
	{
		float y = origin.y + direction.y * t;
		if (y < minPoint.y || y > maxPoint.y) { return false; }
		float z = origin.z + direction.z * t;
		if (z < minPoint.z || z > maxPoint.z) { return false; }
		//aOutIntersectionPoint.x = xn;
		//aOutIntersectionPoint.y = 0.0f;
		//aOutIntersectionPoint.z = 0.0f;
		//return true;
		break;
	}
	case 1:
	{
		float x = origin.x + direction.x * t;
		if (x < minPoint.x || x > maxPoint.x) { return false; }
		float z = origin.z + direction.z * t;
		if (z < minPoint.z || z > maxPoint.z) { return false; }
		//aOutIntersectionPoint.x = 0.0f;
		//aOutIntersectionPoint.y = yn;
		//aOutIntersectionPoint.z = 0.0f;
		//return true;
		break;
	}
	case 2:
	{

		float x = origin.x + direction.x * t;
		if (x < minPoint.x || x > maxPoint.x) { return false; }
		float y = origin.y + direction.y * t;
		if (y < minPoint.y || y > maxPoint.y) { return false; }
		//aOutIntersectionPoint.x = 0.0f;
		//aOutIntersectionPoint.y = 0.0f;
		//aOutIntersectionPoint.z = zn;
		//return true;
		break;
	}
	default:
		break;
	}

	aOutIntersectionPoint = origin + direction * t;
	return true;
}
//
//template<typename T>
//inline bool IntersectionAABBRay(const AABB3D<T>& aAABB , const Vector3<T>& aAABBWorldPosition, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
//{
//	const Vector3<T> rayOrg = aRay.GetOrigin();
//	const Vector3<T> rayDelta = aRay.GetDirection();
//	const Vector3<T> min = aAABB.GetMin() + aAABBWorldPosition;
//	const Vector3<T> max = aAABB.GetMax() + aAABBWorldPosition;
//
//	T xout, yout, zout;
//	if (!IsInside(rayOrg.x, rayDelta.x, min.x, max.x, xout)) return false;
//	if (!IsInside(rayOrg.y, rayDelta.y, min.y, max.y, yout)) return false;
//	if (!IsInside(rayOrg.z, rayDelta.z, min.z, max.z, zout)) return false;
//
//	// Select farthest plane - This is the plane of intersection.
//	int which = 0;
//	T t = xout;
//	if (yout > t)
//	{
//		which = 1;
//		t = yout;
//	}
//	if (zout > t)
//	{
//		which = 2;
//		t = zout;
//	}
//
//	T x = T(), y = T(), z = T();
//
//	switch (which)
//	{
//	case 0: // Intersect with yz plane
//	{
//		y = rayOrg.y + rayDelta.y * t;
//		if (y < min.y || y > max.y) return false;
//		z = rayOrg.z + rayDelta.z * t;
//		if (z < min.z || z > max.z) return false;
//	}
//	break;
//	case 1: // Intersect with xz plane
//	{
//		x = rayOrg.x + rayDelta.x * t;
//		if (x < min.x || x > max.x) return false;
//		z = rayOrg.z + rayDelta.z * t;
//		if (z < min.z || z > max.z) return false;
//	}
//	break;
//	case 2: // Intersect with xy plane
//	{
//		x = rayOrg.x + rayDelta.x * t;
//		if (x < min.x || x > max.x) return false;
//		y = rayOrg.y + rayDelta.y * t;
//		if (y < min.y || y > max.y) return false;
//	}
//	break;
//	}
//
//	aOutIntersectionPoint = Vector3<T>{ x, y, z };
//	return true;
//}
// If the ray intersects the sphere, true is returned, if not, false is returned.
// Any ray starting on the inside is considered to intersect the sphere
template<class T>
inline bool IntersectionSphereRay(const Sphere<T>& aSphere, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
{
	const Vector3<T>& p0 = aRay.GetOrigin();
	const Vector3<T>& d = aRay.GetDirection().GetNormalized();
	const Vector3<T>& c = aSphere.GetCenter();

	const Vector3<T> e = c - p0;
	if (e.GetNormalized().Dot(d) <= 0) return false;

	const T a = e.Dot(d);
	const T b = e.LengthSqr() - a * a;


	if (b > aSphere.GetRadius() * aSphere.GetRadius()) return false;

	const T fArg = aSphere.GetRadius() * aSphere.GetRadius() - e.LengthSqr() + a * a;

	if (fArg < 0) return false;

	const T t = a - sqrt(fArg);

	aOutIntersectionPoint = d * t + p0;

	return true;
}
