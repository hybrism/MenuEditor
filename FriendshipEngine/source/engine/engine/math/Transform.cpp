#include "pch.h"
#include "Transform.h"

Transform::Transform()
{
	myMatrix = DirectX::XMMatrixIdentity();
}

DirectX::XMMATRIX Transform::GetMatrix() const { return myMatrix; }
void Transform::SetMatrix(const DirectX::XMMATRIX& aTransform) { myMatrix = aTransform; }

Vector3f Transform::GetPosition() const
{
	return Vector3f(
		myMatrix.r[3].m128_f32[0],
		myMatrix.r[3].m128_f32[1],
		myMatrix.r[3].m128_f32[2]
	);
}

Vector3f Transform::GetEulerRotation() const
{
	// this might result in gimbal lock
	// chatgpt said this would work if needed

	
	//DirectX::XMMATRIX rotationMatrix = myMatrix;
	//rotationMatrix.r[3].m128_f32[0] = 0;
	//rotationMatrix.r[3].m128_f32[1] = 0;
	//rotationMatrix.r[3].m128_f32[1] = 0;

	//float pitch, yaw, roll;

 //   // Extract individual rotation angles
 //   pitch = asinf(rotationMatrix.r[2].m128_f32[1]);

 //    //Check for gimbal lock at poles
 //   if (cosf(pitch) > 0.001f)
 //   {
 //       yaw = atan2f(rotationMatrix.r[2].m128_f32[0], rotationMatrix.r[2].m128_f32[2]);
 //       roll = atan2f(rotationMatrix.r[0].m128_f32[1], rotationMatrix.r[1].m128_f32[1]);
 //   }
 //   else
 //   {
 //       // Gimbal lock: pitch is near +/- 90 degrees
 //       yaw = 0.0f;
 //       roll = atan2f(-rotationMatrix.r[1].m128_f32[0], rotationMatrix.r[0].m128_f32[0]);
 //   }

	//return { DirectX::XMConvertToDegrees(roll), DirectX::XMConvertToDegrees(pitch), DirectX::XMConvertToDegrees(yaw) };

	
	float yaw = atan2f(myMatrix.r[2].m128_f32[0], myMatrix.r[2].m128_f32[2]);
	float roll = atan2f(myMatrix.r[0].m128_f32[1], myMatrix.r[1].m128_f32[1]);
	float pitch = asin(-myMatrix.r[2].m128_f32[1]);// DENNA ÄR JÄTTE RÄTT!!!!!!!

	return Vector3f(pitch * Rad2Deg, yaw * Rad2Deg, roll * Rad2Deg);
}

Vector3f Transform::GetScale() const 
{
	// TODO: This may give inaccurate results due to rotation?
	// We should technically be able to get the scale from the matrix
	// since scaling happens so infrequently compared to translation and sometimes rotation,
	// we could just save it a variable and use DirectX::XMMatrixDecompose


	// since we should not worry about optimizations for now, this will do
	// however do consider also seperating the scale, rotation and translation into seperate matrices
	// (maybe have the scaling and translation in the same matrix though)

	//return Vector3f(
	//	myMatrix.r[3].m128_f32[0],
	//	myMatrix.r[3].m128_f32[1],
	//	myMatrix.r[3].m128_f32[2]
	//);

	DirectX::XMVECTOR scaleVector;
	DirectX::XMVECTOR rotationVector;
	DirectX::XMVECTOR translationVector;

	DirectX::XMMatrixDecompose(&scaleVector, &rotationVector, &translationVector, myMatrix);

	return Vector3f(
		scaleVector.m128_f32[0],
		scaleVector.m128_f32[1],
		scaleVector.m128_f32[2]
	);
}

void Transform::Translate(const Vector3f& aTranslation)
{
	SetPosition(GetPosition() + aTranslation);
}

void Transform::Rotate(const Vector3f& aEulerRotation)
{
	auto rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(
		aEulerRotation.x * Deg2Rad,
		aEulerRotation.y * Deg2Rad,
		aEulerRotation.z * Deg2Rad
	);

	myMatrix = DirectX::XMMatrixMultiply(rotationMatrix, myMatrix);
}

void Transform::Scale(const Vector3f& aScale)
{
	myMatrix.r[0].m128_f32[0] *= aScale.x;
	myMatrix.r[1].m128_f32[1] *= aScale.y;
	myMatrix.r[2].m128_f32[2] *= aScale.z;
}

Vector3f Transform::GetRight() const
{
	return Vector3f(
		myMatrix.r[0].m128_f32[0],
		myMatrix.r[0].m128_f32[1],
		myMatrix.r[0].m128_f32[2]
	);
}

Vector3f Transform::GetUp() const
{
	return Vector3f(
		myMatrix.r[1].m128_f32[0],
		myMatrix.r[1].m128_f32[1],
		myMatrix.r[1].m128_f32[2]
	);
}

Vector3f Transform::GetForward() const
{
	return Vector3f(
		myMatrix.r[2].m128_f32[0],
		myMatrix.r[2].m128_f32[1],
		myMatrix.r[2].m128_f32[2]
	);
}

void Transform::SetPosition(const Vector3f& aPosition)
{
	myMatrix.r[3].m128_f32[0] = aPosition.x;
	myMatrix.r[3].m128_f32[1] = aPosition.y;
	myMatrix.r[3].m128_f32[2] = aPosition.z;
}

void Transform::SetEulerAngles(const Vector3f& aEulerAngles)
{
	DirectX::XMVECTOR scaleVector;
	DirectX::XMVECTOR rotationVector;
	DirectX::XMVECTOR translationVector;

	DirectX::XMMatrixDecompose(&scaleVector, &rotationVector, &translationVector, myMatrix);

	myMatrix = DirectX::XMMatrixScalingFromVector(scaleVector) * 
		DirectX::XMMatrixRotationRollPitchYaw(aEulerAngles.x * Deg2Rad, aEulerAngles.y * Deg2Rad, aEulerAngles.z * Deg2Rad) *
		DirectX::XMMatrixTranslationFromVector(translationVector);
}

void Transform::SetScale(const Vector3f& aScale)
{
	DirectX::XMVECTOR scaleVector;
	DirectX::XMVECTOR rotationVector;
	DirectX::XMVECTOR translationVector;

	DirectX::XMMatrixDecompose(&scaleVector, &rotationVector, &translationVector, myMatrix);

	myMatrix = DirectX::XMMatrixScaling(aScale.x, aScale.y, aScale.z) * 
		DirectX::XMMatrixRotationQuaternion(rotationVector) * 
		DirectX::XMMatrixTranslationFromVector(translationVector);
}
