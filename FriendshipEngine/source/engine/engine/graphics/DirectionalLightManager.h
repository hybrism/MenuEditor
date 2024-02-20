#pragma once
#include <wrl/client.h>
//#include "DepthBuffer.h"
//#include "../math/Vector.h"
//
//class Camera;
//
//using Microsoft::WRL::ComPtr;
//struct ID3D11Buffer;
//
//// TODO: This could be converted into a general light manager
//class DirectionalLightManager
//{
//public:
//	DirectionalLightManager();
//	~DirectionalLightManager();
//
//	void Init(
//		const Vector3f& aDirection = { 0.0f, -1.0f, 0.0f },
//		const Vector4f& aColor = { 0.82f, 0.42f, 0.0f, 1.0f }
//	);
//
//	Vector3f GetDirection() { return myDirection; }
//	Vector4f GetColor() { return myColor; }
//
//	void SetDirectionAndColor(const Vector3f& aDirection, const Vector4f& aColor);
//	void SetDirection(const Vector3f& aDirection);
//	void SetColor(const Vector4f& aColor);
//	void SetColor(const Vector3f& aColor);
//
//	void BeginShadowRendering();
//	void EndShadowRendering();
//	void EndOfRenderPassReset();
//
//	void SetDirCamerasPos(const Vector3f& aPos);
//
//	void UpdateBuffer();
//	void DEBUG_ImGuiChangeValues();
//
//private:
//	DepthBuffer myShadowMapDepthBuffer;
//
//	Vector4f myColor;
//	Vector3f myDirection;
//
//	ComPtr<ID3D11Buffer> myDirectionalLightBuffer;
//	Camera* myDirectionalLightCamera;
//};