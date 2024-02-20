#pragma once
#include "Matrix4x4.h"
#include "Vector.h"


template <class T>
class Matrix3x3
{

private:
	T myContainer[3][3];
public:
	// Creates the identity matrix.
	Matrix3x3<T>()
	{
		for (unsigned int y = 0; y < 3; y++)
		{
			for (unsigned int x = 0; x < 3; x++)
			{
				myContainer[y][x] = T() + x == y ? 1 : T();
			}
		}
	};

	// Copy Constructor.
	Matrix3x3<T>(const Matrix3x3<T>& aMatrix)
	{
		(*this) = aMatrix;
	};

	// Copies the top left 3x3 part of the Matrix4x4.
	Matrix3x3<T>(const Matrix4x4<T>& aMatrix)
	{
		for (unsigned int y = 0; y < 3; y++)
		{
			for (unsigned int x = 0; x < 3; x++)
			{
				myContainer[y][x] = aMatrix(y + 1, x + 1);
			}
		}
	};

	Matrix3x3<T>& operator=(const Matrix3x3<T>& aMatrix)
	{
		for (unsigned int y = 1; y <= 3; y++)
		{
			for (unsigned int x = 1; x <= 3; x++)
			{
				(*this)(y, x) = aMatrix(y, x);
			}
		}
		return *this;
	}

	// () operator for accessing element (row, column) for read/write or read, respectively.
	T& operator()(const unsigned int aRow, const unsigned int aColumn)
	{
		return myContainer[aRow - 1][aColumn - 1];
	};

	const T& operator()(const unsigned int aRow, const unsigned int aColumn) const
	{
		return myContainer[aRow - 1][aColumn - 1];
	};

	// Static functions for creating rotation matrices.
	static Matrix3x3<T> CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix3x3<T> rotationMatrix;
		rotationMatrix(2, 2) = cos(aAngleInRadians);
		rotationMatrix(2, 3) = sin(aAngleInRadians);

		rotationMatrix(3, 2) = -sin(aAngleInRadians);
		rotationMatrix(3, 3) = cos(aAngleInRadians);

		return rotationMatrix;
	};

	static Matrix3x3<T> CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix3x3<T> rotationMatrix;
		rotationMatrix(1, 1) = cos(aAngleInRadians);
		rotationMatrix(1, 3) = -sin(aAngleInRadians);

		rotationMatrix(3, 1) = sin(aAngleInRadians);
		rotationMatrix(3, 3) = cos(aAngleInRadians);

		return rotationMatrix;
	};

	static Matrix3x3<T> CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix3x3<T> rotationMatrix;
		rotationMatrix(1, 1) = cos(aAngleInRadians);
		rotationMatrix(1, 2) = sin(aAngleInRadians);

		rotationMatrix(2, 1) = -sin(aAngleInRadians);
		rotationMatrix(2, 2) = cos(aAngleInRadians);

		return rotationMatrix;
	};

	// Static function for creating a transpose of a matrix.
	static Matrix3x3<T> Transpose(const Matrix3x3<T>& aMatrixToTranspose)
	{
		Matrix3x3<T> container = Matrix3x3<T>(aMatrixToTranspose);

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				container(j + 1, i + 1) = aMatrixToTranspose(i + 1, j + 1);
			}
		}

		return container;
	};
};

template <class T> Matrix3x3<T> operator+(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
{
	Matrix3x3<T> result;
	for (unsigned int y = 1; y <= 3; y++)
	{
		for (unsigned int x = 1; x <= 3; x++)
		{
			result(y, x) = aMatrix0(y, x) + aMatrix1(y, x);
		}
	}
	return result;
}

template <class T> Matrix3x3<T> operator+=(Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
{
	aMatrix0 = aMatrix0 + aMatrix1;
	return aMatrix0;
}

template <class T> Matrix3x3<T> operator-(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
{
	Matrix3x3<T> result;
	for (unsigned int y = 1; y <= 3; y++)
	{
		for (unsigned int x = 1; x <= 3; x++)
		{
			result(y, x) = aMatrix0(y, x) - aMatrix1(y, x);
		}
	}
	return result;
}

template <class T> Matrix3x3<T> operator-=(Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
{
	aMatrix0 = aMatrix0 - aMatrix1;
	return aMatrix0;
}

template <class T> Matrix3x3<T> operator*(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
{
	Matrix3x3<T> result;
	for (unsigned int i = 1; i <= 3; i++)
	{
		for (unsigned int j = 1; j <= 3; j++)
		{
			T sum = T();

			for (unsigned int k = 1; k <= 3; k++)
			{
				sum += aMatrix0(i, k) * aMatrix1(k, j);
			}

			result(i, j) = sum;
		}
	}
	return result;
}

template <class T> Matrix3x3<T> operator*=(Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
{
	aMatrix0 = aMatrix0 * aMatrix1;
	return aMatrix0;
}

template <class T> Vector3<T> operator*(const Vector3<T>& aVector, const Matrix3x3<T>& aMatrix)
{
	return Vector3<T>(
		aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1),
		aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2),
		aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3)
	);
}

template <class T> bool operator==(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
{
	for (unsigned int y = 1; y <= 3; y++)
	{
		for (unsigned int x = 1; x <= 3; x++)
		{
			if (aMatrix0(y, x) != aMatrix1(y, x)) return false;
		}
	}
	return true;
}
