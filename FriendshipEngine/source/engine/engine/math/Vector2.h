#pragma once

template <class T>
class Vector2
{
public:
#pragma warning (disable : 4201) // Nonstandard nameless struct/union.
	union
	{
		T myValues[2];
		struct { T x; T y; };
	};

	//Creates a null-vector
	Vector2<T>()
	{
		x = 0;
		y = 0;
	};

	//Creates a vector (aX, aY, aZ)
	Vector2<T>(const T& aX, const T& aY)
	{
		x = aX;
		y = aY;
	};

	template <class U>
	Vector2<T>(const Vector2<U>& aValue)
	{
		x = static_cast<T>(aValue.x);
		y = static_cast<T>(aValue.y);
	};

	//Copy constructor (compiler generated)
	Vector2<T>(const Vector2<T>& aVector) = default;
	//Assignment operator (compiler generated)
	Vector2<T>& operator=(const Vector2<T>& aVector2) = default;
	//Destructor (compiler generated)
	~Vector2<T>() = default;
	//Returns the squared length of the vector
	T LengthSqr() const
	{
		return x * x + y * y;
	};
	//Returns the length of the vector
	T Length() const
	{
		return std::sqrt(Vector2<T>::LengthSqr());
	};
	//Returns a normalized copy of this
	Vector2<T> GetNormalized() const
	{
		if (Length() == 0) return Vector2<T>();

		return *this / Length();
	};
	//Normalizes the vector
	void Normalize()
	{
		if (Length() == 0) return;

		*this /= Length();
	};
	//LERP
	Vector2<T> LerpVec2(const Vector2<T>& start, const Vector2<T>& end, float t) const
	{
		return ::LerpVec2(start, end, t);
	}
	//Returns the dot product of this and aVector
	T Dot(const Vector2<T>& aVector) const
	{
		return x * aVector.x + y * aVector.y;
	};
};
//Returns the vector sum of aVector0 and aVector1
template <class T> Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
{
	return Vector2<T>(
		aVector0.x + aVector1.x,
		aVector0.y + aVector1.y
	);
}
//Returns the vector difference of aVector0 and aVector1
template <class T> Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
{
	return Vector2<T>(
		aVector0.x - aVector1.x,
		aVector0.y - aVector1.y
	);
}
//Returns the vector aVector multiplied by the scalar aScalar
template <class T> Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
{
	return Vector2<T>(
		aVector.x * aScalar,
		aVector.y * aScalar
	);
}
//Returns the vector aVector multiplied by the scalar aScalar
template <class T> Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
{
	return Vector2<T>(
		aVector.x * aScalar,
		aVector.y * aScalar
	);
}
template <class T> Vector2<T> LerpVec2(const Vector2<T>& start, const Vector2<T>& end, float t)
{

	return Vector2<T>(
		start.x + (end.x - start.x) * t,
		start.y + (end.y - start.y) * t
	);
}
//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
{
	return Vector2<T>(
		aVector.x / aScalar,
		aVector.y / aScalar
	);
}
//Equivalent to setting aVector0 to (aVector0 + aVector1)
template <class T> void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
{
	aVector0.x += aVector1.x;
	aVector0.y += aVector1.y;
}
//Equivalent to setting aVector0 to (aVector0 - aVector1)
template <class T> void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
{
	aVector0.x -= aVector1.x;
	aVector0.y -= aVector1.y;
}
//Equivalent to setting aVector to (aVector * aScalar)
template <class T> void operator*=(Vector2<T>& aVector, const T& aScalar)
{
	aVector.x *= aScalar;
	aVector.y *= aScalar;
}
//Equivalent to setting aVector to (aVector / aScalar)
template <class T> void operator/=(Vector2<T>& aVector, const T& aScalar)
{
	aVector.x /= aScalar;
	aVector.y /= aScalar;
}

