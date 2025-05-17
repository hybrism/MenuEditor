#pragma once

template <class T>
class Vector3
{
public:
#pragma warning (disable : 4201) // Nonstandard nameless struct/union.
	union
	{
		T myValues[3];
		struct { T x; T y; T z; };
	};
	//Creates a null-vector
	Vector3<T>()
	{
		x = 0;
		y = 0;
		z = 0;
	};

	//Creates a vector (aX, aY, aZ)
	Vector3<T>(const T& aX, const T& aY, const T& aZ)
	{
		x = aX;
		y = aY;
		z = aZ;
	};

	template <class U>
	Vector3<T>(const Vector3<U>& aValue)
	{
		x = static_cast<T>(aValue.x);
		y = static_cast<T>(aValue.y);
		z = static_cast<T>(aValue.z);
	};

	//Copy constructor (compiler generated)
	Vector3<T>(const Vector3<T>& aVector) = default;
	//Assignment operator (compiler generated)
	Vector3<T>& operator=(const Vector3<T>& aVector3) = default;
	//Destructor (compiler generated)
	~Vector3<T>() = default;

	//Gets the distance between 2 Vector3
	static T Distance(const Vector3<T>& aVector0, const Vector3<T>& aVector1);
	static Vector3<T> Cross(const Vector3<T>& aVector0, const Vector3<T>& aVector1);
	static T Dot(const Vector3<T>& aVector0, const Vector3<T>& aVector1);


	//Returns the squared length of the vector
	T LengthSqr() const
	{
		return x * x + y * y + z * z;
	};
	//Returns the length of the vector
	T Length() const
	{
		return std::sqrt(Vector3<T>::LengthSqr());
	};
	//Returns a normalized copy of this
	Vector3<T> GetNormalized() const
	{
		if (Length() == 0) return Vector3<T>();

		return *this / Length();
	};

	//Normalizes the vector
	void Normalize()
	{
		if (Length() == 0) return;

		*this /= Length();
	};

	static Vector3<T> Project(const Vector3<T>& aVector, const Vector3<T>& aDirection)
	{
		float dot = aVector.Dot(aDirection) / aDirection.LengthSqr();
		return aDirection * dot;
	}

	static Vector3<T> Lerp(const Vector3<T>& start, const Vector3<T>& end, float t)
	{
		return Vector3<T>(
			start.x + (end.x - start.x) * t,
			start.y + (end.y - start.y) * t,
			start.z + (end.z - start.z) * t
		);
	}

	Vector3<T> Lerp(const Vector3<T>& end, float t) const
	{
		return Lerp(*this, end, t);
	}

	T Dot(const Vector3<T>& aVector) const
	{
		//T a = (x * aVector.x) + (y * aVector.y) + (z * aVector.z);
		T ax = (x * aVector.x);
		T ay = (y * aVector.y);
		T az = (z * aVector.z);
		return ax + ay + az;
	};

	//Returns the cross product of this and aVector
	Vector3<T> Cross(const Vector3<T>& aVector) const
	{
		return Vector3<T>(
			y * aVector.z - z * aVector.y,
			z * aVector.x - x * aVector.z,
			x * aVector.y - y * aVector.x
		);
	};

	static Vector3<T> Zero()
	{
		return Vector3<T>(0, 0, 0);
	}

	static Vector3<T> Up()
	{
		return Vector3<T>(0, 1, 0);
	}

	static Vector3<T> Right()
	{
		return Vector3<T>(1, 0, 0);
	}

	static Vector3<T> Forward()
	{
		return Vector3<T>(0, 0, 1);
	}
};

template <class T> Vector3<T> Vector3<T>::Cross(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	return Vector3<T>(
		aVector0.y * aVector1.z - aVector0.z * aVector1.y,
		aVector0.z * aVector1.x - aVector0.x * aVector1.z,
		aVector0.x * aVector1.y - aVector0.y * aVector1.x
	);
};

template <class T> T Vector3<T>::Distance(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	const Vector3<T> direction = aVector1 - aVector0;
	return direction.Length();
};

template <class T> T Vector3<T>::Dot(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	T ax = (aVector0.x * aVector1.x);
	T ay = (aVector0.y * aVector1.y);
	T az = (aVector0.z * aVector1.z);
	return ax + ay + az;
};

//Returns the vector sum of aVector0 and aVector1
template <class T> Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	return Vector3<T>(
		aVector0.x + aVector1.x,
		aVector0.y + aVector1.y,
		aVector0.z + aVector1.z
	);
}
//Returns the vector difference of aVector0 and aVector1
template <class T> Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	return Vector3<T>(
		aVector0.x - aVector1.x,
		aVector0.y - aVector1.y,
		aVector0.z - aVector1.z
	);
}
//Returns the vector aVector multiplied by the scalar aScalar
template <class T> Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
{
	return Vector3<T>(
		aVector.x * aScalar,
		aVector.y * aScalar,
		aVector.z * aScalar
	);
}
//Returns the vector aVector multiplied by the scalar aScalar
template <class T> Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
{
	return Vector3<T>(
		aVector.x * aScalar,
		aVector.y * aScalar,
		aVector.z * aScalar
	);
}
//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
template <class T> Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
{
	return Vector3<T>(
		aVector.x / aScalar,
		aVector.y / aScalar,
		aVector.z / aScalar
	);
}
//Equivalent to setting aVector0 to (aVector0 + aVector1)
template <class T> void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	aVector0.x += aVector1.x;
	aVector0.y += aVector1.y;
	aVector0.z += aVector1.z;
}
//Equivalent to setting aVector0 to (aVector0 - aVector1)
template <class T> void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	aVector0.x -= aVector1.x;
	aVector0.y -= aVector1.y;
	aVector0.z -= aVector1.z;
}
//Equivalent to setting aVector to (aVector * aScalar)
template <class T> void operator*=(Vector3<T>& aVector, const T& aScalar)
{
	aVector.x *= aScalar;
	aVector.y *= aScalar;
	aVector.z *= aScalar;
}
//Equivalent to setting aVector to (aVector / aScalar)
template <class T> void operator/=(Vector3<T>& aVector, const T& aScalar)
{
	aVector.x /= aScalar;
	aVector.y /= aScalar;
	aVector.z /= aScalar;
}
//Equivalent to setting aVector to (aVector / aScalar)
template <class T> Vector3<T> operator*(Vector3<T>& aVector, const Vector3<T>& aOther)
{
	return Vector3<T>(
		aVector.x * aOther.x,
		aVector.y * aOther.y,
		aVector.z * aOther.z
	);
}

//Equivalent to setting aVector to (aVector / aScalar)
template <class T> void operator*=(Vector3<T>& aVector, const Vector3<T>& aOther)
{
	aVector.x *= aOther.x;
	aVector.y *= aOther.y;
	aVector.z *= aOther.z;
}

template <class T> bool operator!=(Vector3<T>& aVector, const Vector3<T>& aOther)
{
	return aVector.x != aOther.x || aVector.y != aOther.y || aVector.z != aOther.z;

}


