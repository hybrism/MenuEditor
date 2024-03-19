#pragma once
#include "Vector.h"
#include "Math.h"
#include <DirectXMath.h>

class Transform
{
public:
	Transform();
	Transform(const DirectX::XMMATRIX& aTransform) { myMatrix = aTransform; }
	Transform& operator=(const DirectX::XMMATRIX& aMatrix)
	{
		myMatrix = aMatrix;
		return *this;
	}
	~Transform() = default;

	DirectX::XMMATRIX GetMatrix() const;
	void SetMatrix(const DirectX::XMMATRIX& aTransform);

	Vector3f GetPosition() const;
	Vector3f GetEulerRotation() const;
	Vector3f GetScale() const;

	void Translate(const Vector3f& aTranslation);
	void Rotate(const Vector3f& aEulerRotation);
	void Scale(const Vector3f& aScale);

	Vector3f GetRight() const;
	Vector3f GetUp() const;
	Vector3f GetForward() const;

	void SetPosition(const Vector3f& aPosition);
	void SetEulerAngles(const Vector3f& aEulerAngles);
	void SetScale(const Vector3f& aScale);

	Transform operator*=(const Transform& aTransform)
	{
		myMatrix = DirectX::XMMatrixMultiply(myMatrix, aTransform.GetMatrix());
		return *this;
	}
private:
	DirectX::XMMATRIX myMatrix;
};
