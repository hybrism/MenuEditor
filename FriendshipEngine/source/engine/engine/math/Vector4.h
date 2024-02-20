#pragma once

template <class T>
class Vector4
{
public:
#pragma warning (disable : 4201) // Nonstandard nameless struct/union.
	union
	{
		T myValues[4];
		struct { T x; T y; T z; T w; };
	};
	//Creates a null-vector
	Vector4<T>()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	};

	//Creates a vector (aX, aY, aZ)
	Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& aW)
	{
		x = aX;
		y = aY;
		z = aZ;
		w = aW;
	};

	template <class U>
	Vector4<T>(const Vector4<U>& aValue)
	{
		x = static_cast<T>(aValue.x);
		y = static_cast<T>(aValue.y);
		z = static_cast<T>(aValue.z);
		w = static_cast<T>(aValue.w);
	};

	//Copy constructor (compiler generated)
	Vector4<T>(const Vector4<T>& aVector) = default;
	//Assignment operator (compiler generated)
	Vector4<T>& operator=(const Vector4<T>& aVector4) = default;

	bool operator==(const Vector4<T>& other) const {
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}
	//Destructor (compiler generated)
	~Vector4<T>() = default;
	//Returns the squared length of the vector
	T LengthSqr() const
	{
		return x * x + y * y + z * z + w * w;
	};
	//Returns the length of the vector
	T Length() const
	{
		return std::sqrt(Vector4<T>::LengthSqr());
	};
	//Returns a normalized copy of this
	Vector4<T> GetNormalized() const
	{
		if (Length() == 0) return Vector4<T>();

		return *this / Length();
	};
	//Normalizes the vector
	void Normalize()
	{
		if (Length() == 0) return;

		*this /= Length();
	};
	//Returns the dot product of this and aVector
	T Dot(const Vector4<T>& aVector) const
	{
		return x * aVector.x + y * aVector.y + z * aVector.z + w * aVector.w;
	};
};
//Returns the vector sum of aVector0 and aVector1
template <class T> Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
{
	return Vector4<T>(
		aVector0.x + aVector1.x,
		aVector0.y + aVector1.y,
		aVector0.z + aVector1.z,
		aVector0.w + aVector1.w
	);
}
//Returns the vector difference of aVector0 and aVector1
template <class T> Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
{
	return Vector4<T>(
		aVector0.x - aVector1.x,
		aVector0.y - aVector1.y,
		aVector0.z - aVector1.z,
		aVector0.w - aVector1.w
	);
}
//Returns the vector aVector multiplied by the scalar aScalar
template <class T> Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
{
	return Vector4<T>(
		aVector.x * aScalar,
		aVector.y * aScalar,
		aVector.z * aScalar,
		aVector.w * aScalar
	);
}
//Returns the vector aVector multiplied by the scalar aScalar
template <class T> Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
{
	return Vector4<T>(
		aVector.x * aScalar,
		aVector.y * aScalar,
		aVector.z * aScalar,
		aVector.w * aScalar
	);
}
//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
template <class T> Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
{
	return Vector4<T>(
		aVector.x / aScalar,
		aVector.y / aScalar,
		aVector.z / aScalar,
		aVector.w / aScalar
	);
}
//Equivalent to setting aVector0 to (aVector0 + aVector1)
template <class T> void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
{
	aVector0.x += aVector1.x;
	aVector0.y += aVector1.y;
	aVector0.z += aVector1.z;
	aVector0.w += aVector1.w;
}
//Equivalent to setting aVector0 to (aVector0 - aVector1)
template <class T> void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
{
	aVector0.x -= aVector1.x;
	aVector0.y -= aVector1.y;
	aVector0.z -= aVector1.z;
	aVector0.w -= aVector1.w;
}
//Equivalent to setting aVector to (aVector * aScalar)
template <class T> void operator*=(Vector4<T>& aVector, const T& aScalar)
{
	aVector.x *= aScalar;
	aVector.y *= aScalar;
	aVector.z *= aScalar;
	aVector.w *= aScalar;
}
//Equivalent to setting aVector to (aVector / aScalar)
template <class T> void operator/=(Vector4<T>& aVector, const T& aScalar)
{
	aVector.x /= aScalar;
	aVector.y /= aScalar;
	aVector.z /= aScalar;
	aVector.w /= aScalar;
}

template<class T>
inline bool operator==(Vector4<T>& a, Vector4<T>& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}
