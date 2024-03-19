#pragma once
#pragma once
#include <wrl/client.h>
#include "../math/Matrix.h"
#include <DirectXMath.h>
#define MAX_POINTLIGHTS 32

using Microsoft::WRL::ComPtr;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;

// TODO: Change all matrices to XMMatrix
struct FrameBufferData
{
	Vector4<float> resolution; //myResolution.x = screen width, myResolution.y = screen height, myResolution.z = unset, myResolution.w = unset
	DirectX::XMMATRIX worldToClipMatrix;
	DirectX::XMMATRIX modelToWorld;
	float nearPlane;
	float farPlane;
	float unused[2];
};

struct ObjectBufferData
{
	DirectX::XMMATRIX objectTransform;
};

// TODO: optimize memory useage
struct InputBufferData
{
	float time;
	float deltaTime;
	unsigned int renderMode; // GBufferTexture
	float unused[1];
};


struct LightBufferData
{
	Vector3<float> directionalLightsDirection;
	float directionalLightsIntensity;
	Vector3<float> directionalLightsColor;
	float ambientLightIntensity;
	DirectX::XMMATRIX dLightShadowSpaceToWorldSpace;

	struct PointLights
	{
		Vector3<float> pointLightsPosition;
		float pointLightsRange; 
		Vector3<float> pointLightsColor;
		float pointLightsIntensity; 
	}myPointLights[32];

	int amountOfPointLights;
	int useShadows = 0; //   1 = NO   :  0 = YES      Math reasons.. ask Oscar Ruselius Karl Onsomkit Ekarnan
	float unused[2];
};

struct PointLightSphereColorData
{
	Vector4<float>  SphereColor;
};



// TODO: optimize memory useage
struct PostProcessBufferData
{
	int downScaleLevel = 0;
	float alphaBlendLevel = 0.25f;
	float saturation = 1.1f;
	float exposure = 0.0f;

	float contrast = 1.0f;
	float vignetteInner = 0.3f;
	float vignetteOuter = 1.6f;
	float vignetteStrength = 0.0f;

	Vector3f tint = { 1.f, 1.f, 1.f };
	float vignetteCurvature = 0.5f;

	Vector3f blackPoint = { 0.f , 0.f, 0.f };
	float trash3 = 0.0f;

	Vector3f vignetteColor = { 1.f, 0.f, 0.f };
	float trash = 0.0f;
};
