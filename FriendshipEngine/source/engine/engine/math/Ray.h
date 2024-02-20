#pragma once
#include "Vector3.h"


template<class T>
class Ray
{
private:
	Vector3<T> myOrigin;
	Vector3<T> myDirection;
public:
	// Default constructor: there is no ray, the origin and direction are the zero vector.
	Ray() : myOrigin(), myDirection()
	{

	};
	// Copy constructor.
	Ray(const Ray<T>& aRay)
	{
		myOrigin = aRay.myOrigin;
		myDirection = aRay.myDirection;
	};
	// Constructor that takes two points that define the ray, the direction is
	// aDirectionm and the origin is aOrigin.
	Ray(const Vector3<T>& aOrigin, const Vector3<T>& aDirection)
	{
		InitWithOriginAndDirection(aOrigin, aDirection);
	};
	// Init the ray with two points, the same as the constructor above.
	void InitWith2Points(const Vector3<T>& aOrigin, const Vector3<T>& aPoint)
	{
		myOrigin = aOrigin;
		myDirection = aPoint - aOrigin; //? andra hållet ?
	};
	// Init the ray with an origin and a direction.
	void InitWithOriginAndDirection(const Vector3<T>& aOrigin, const Vector3<T>& aDirection)
	{
		myOrigin = aOrigin;
		myDirection = aDirection;
	};

	const Vector3<T>& GetDirection() const
	{
		return myDirection;
	}

	const Vector3<T>& GetOrigin() const
	{
		return myOrigin;
	}
};

