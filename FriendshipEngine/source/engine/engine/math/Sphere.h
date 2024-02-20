#pragma once
#include "Vector3.h"

template<class T>
class Sphere
{
private:
	Vector3<T> myCenter;
	T myRadius;
public:
	// Default constructor: there is no sphere, the radius is zero and the position is
// the zero vector.
	Sphere() : myCenter(), myRadius() { };
	// Copy constructor.
	Sphere(const Sphere<T>& aSphere)
	{
		myCenter = aSphere.myCenter;
		myRadius = aSphere.myRadius;
	};
	// Constructor that takes the center position and radius of the sphere.
	Sphere(const Vector3<T>& aCenter, T aRadius)
	{
		InitWithCenterAndRadius(aCenter, aRadius);
	};
	// Init the sphere with a center and a radius, the same as the constructor above.
	void InitWithCenterAndRadius(const Vector3<T>& aCenter, T aRadius)
	{
		myCenter = aCenter;
		myRadius = aRadius;
	};
	// Returns whether a point is inside the sphere: it is inside when the point is on the
	// sphere surface or inside of the sphere.
	bool IsInside(const Vector3<T>& aPosition) const
	{
		return Vector3<T>(myCenter - aPosition).LengthSqr() <= myRadius * myRadius;
	};

	const Vector3<T>& GetCenter() const
	{
		return myCenter;
	}

	const T& GetRadius() const
	{
		return myRadius;
	}

	const Vector3<T> GetNormalFromPoint(const Vector3<T>& aPoint) const
	{
		Vector3<T> normal = Vector3<T>(aPoint - myCenter);
		normal.Normalize();
		return normal;
	}
};

