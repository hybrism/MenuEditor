#pragma once


#include "../../../engine/math/Vector3.h"
#include "../../../engine/math/Vector4.h"
#include "../../../engine/math/VectorFwd.h"
#include "../../../engine/math/Transform.h"

#include <engine\graphics\DepthBuffer.h>

class DepthBuffer;
class Camera;


struct PointLight
{
	PointLight() = default;
	PointLight(Vector3f aPosition, Vector3f aColor, float aRange, float aIntensity)
	{
		myPos = aPosition;
		myRange = aRange;
		myColor = aColor;
		myIntensity = aIntensity;
	};

	Vector3f myPos;
	float myRange;
	Vector3f myColor;
	float myIntensity;
};

struct DirectionalLight
{
	DirectionalLight() = default;
	DirectionalLight(Vector3f aDirection, Vector3f aColor, float aIntensity)
	{
		myColor = aColor;
		myDirection = aDirection;
		myIntensity = aIntensity;

		myDirectionalLightCamera = nullptr;
	}

	Vector3f myColor;
	Vector3f myDirection;
	float myIntensity = -1.f;
	float myAmbientIntensity = 0.75f;
	Transform myTransform;
	Vector3<float> myEuler;

	DepthBuffer myShadowMapDepthBuffer;
	Camera* myDirectionalLightCamera = nullptr;
};