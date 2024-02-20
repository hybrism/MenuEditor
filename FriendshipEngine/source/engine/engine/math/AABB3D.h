#pragma once
#include "Vector3.h"

template<class T>
class AABB3D
{
private:
	Vector3<T> myMin;
	Vector3<T> myMax;
public:
	// Default constructor: there is no AABB, both min and max points are the zero vector.
	AABB3D() : myMin(), myMax()
	{

	};
	// Copy constructor.
	AABB3D(const AABB3D<T>& aAABB3D)
	{
		myMin = aAABB3D.myMin;
		myMax = aAABB3D.myMax;
	};
	// Constructor taking the positions of the minimum and maximum corners.
	AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		InitWithMinAndMax(aMin, aMax);
	};
	// Init the AABB with the positions of the minimum and maximum corners, same as
	// the constructor above.
	void InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		myMin = aMin;
		myMax = aMax;
	};
	// Returns whether a point is inside the AABB: it is inside when the point is on any
	// of the AABB's sides or inside of the AABB.
	bool IsInside(const Vector3<T>& aPosition) const
	{
		if (myMax.x < aPosition.x) return false;
		if (myMin.x > aPosition.x) return false;
		if (myMax.y < aPosition.y) return false;
		if (myMin.y > aPosition.y) return false;
		if (myMax.z < aPosition.z) return false;
		if (myMin.z > aPosition.z) return false;
		return true; // AABB överlappar
	};

	const Vector3<T>& GetMin() const
	{
		return myMin;
	};
	const Vector3<T>& GetMax() const
	{
		return myMax;
	};
};