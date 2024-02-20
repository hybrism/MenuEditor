#pragma once
#include "Vector2.h"

template<class T>
class Line
{
public:
	// Default constructor: there is no line, the normal is the zero vector.
	Line();
	// Copy constructor.
	Line(const Line <T>& aLine);
	// Constructor that takes two points that define the line, the direction is aPoint1 -aPoint0.
	Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);
	// Init the line with two points, the same as the constructor above.
	void InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1);
	// Init the line with a point and a direction.
	void InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>&
		aDirection);
	// Returns whether a point is inside the line: it is inside when the point is on the line or on the side the normal is pointing away from.
	bool IsInside(const Vector2<T>& aPosition) const;
	// Returns the direction of the line.
	const Vector2<T>& GetDirection() const;
	// Returns the normal of the line, whic
	const Vector2<T> GetNormal() const;
private:
	Vector2<T> myPoint;
	Vector2<T> myDirection;
};
template<class T>
inline Line<T>::Line()
{
	myDirection = { 0, 0 };
	myPoint = { 0, 0 };
}
template<class T>
inline Line<T>::Line(const Line<T>& aLine)
{
	myDirection = aLine.myDirection;
	myPoint = aLine.myPoint;
}
template<class T>
inline Line<T>::Line(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
{
	InitWith2Points(aPoint0, aPoint1);
}
template<class T>
inline void Line<T>::InitWith2Points(const Vector2<T>& aPoint0, const Vector2<T>& aPoint1)
{
	myDirection = aPoint1 - aPoint0;
	myPoint = aPoint0;
}
template<class T>
inline void Line<T>::InitWithPointAndDirection(const Vector2<T>& aPoint, const Vector2<T>& aDirection)
{
	myDirection = aDirection;
	myPoint = aPoint;
}
template<class T>
inline bool Line<T>::IsInside(const Vector2<T>& aPosition) const
{
	return (aPosition - myPoint).Dot(GetNormal()) <= 0;
}
template<class T>
inline const Vector2<T>& Line<T>::GetDirection() const
{
	return myDirection;
}
template<class T>
inline const Vector2<T> Line<T>::GetNormal() const
{
	return Vector2<T>{ -myDirection.y, myDirection.x };
}

