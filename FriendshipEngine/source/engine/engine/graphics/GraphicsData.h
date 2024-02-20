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
	Matrix4x4<float> worldToClipMatrix;
	Matrix4x4<float> modelToWorld;
	Vector4<float> cameraPosition;
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

//struct PointLightBufferData
//{
//	struct PointLights
//	{
//		Vector4<float> pos;
//		Vector4<float> color;
//
//		float range; // TODO: We can bake range into alpha chanel of color since it is not used and save performance
//		float unused[3];
//	}myPointLights[8];  //TO DO Ändra till ett max senare
//
//	int amountOfPointLights;
//	alignas(4) bool isAdditiveStateOn; // TODO: This is really inefficient, do we really need this?
//	float unused[2];
//};	
//
//struct DirectionalLightBufferData
//{
//	Vector4<float> dLightDir;
//	Vector4<float> dLightColor;
//	Matrix4x4<float> dLightShadowSpaceToWorldSpace;
//};

struct LightBufferData
{
	Vector4<float> directionalLightsDirection;
	Vector3<float> directionalLightsColor;
	float directionalLightsIntensity;
	Matrix4x4<float> dLightShadowSpaceToWorldSpace;

	struct PointLights
	{
		Vector3<float> pointLightsPosition;
		float pointLightsRange; 
		Vector3<float> pointLightsColor;
		float pointLightsIntensity; 
	}myPointLights[MAX_POINTLIGHTS];

	int amountOfPointLights;
	float unused[3];

	//alignas(4) bool isAdditiveStateOn; // TODO: This is really inefficient, do we really need this?
};

//struct ShadowMapCameraSpaceBufferData
//{
//	Matrix4x4<float> directionalLightsShadowSpaceToWorldSpace;
//};


// TODO: optimize memory useage
struct PostProcessBufferData
{
	int DownScaleLevel;
	float AlphaBlendLevel;
	float buffSaturation;
	float buffExposure;

	float buffContrast[3];
	float bufftrash1;

	float buffTint[3];
	float bufftrash2;

	float buffBlackPoint[3];
	float bufftrash3;
};

//struct RenderTarget
//{
//	ComPtr<ID3D11RenderTargetView> renderTargetView;
//	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
//};