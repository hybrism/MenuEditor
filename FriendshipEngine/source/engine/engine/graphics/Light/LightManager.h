#pragma once
#include "LightStructs.h"
//#include <graphics\GraphicsData.h>
#include <assets\FactoryStructs.h>
#include <vector>
#include "../GraphicsData.h"
#include "../RenderTarget.h"
	
struct RenderTarget;
struct ID3D11Buffer;
class SharedMesh;
class VertexShader;
class PixelShader;
class Camera;

class LightManager
{
public:
	LightManager();
	~LightManager();

	void Init(
		const Vector3f& aDirection = { 0.0f, -1.0f, 0.0f },
		const Vector4f& aColor = { 255.f, 255.f, 255.f, 1.0f },
		const Vector3f& aPosition = { 0.0f, 0.0f, 0.0f }
	);
	
	void SetLightsOnBuffer();
	void ImportLights();

	void LightRender();

	//PointLight
	void RenderPointLightSpheres();
	void RenderPointLightsBounds();

	//DirectionalLight
	void RenderDirectionalLight();
	void SetDirectionalLightsDirection(const Vector3f& aDirection);
	void SetDirectionalLightsColor(const Vector4f& aColor);
	void SetDirectionalLightsCameraToPosition(const Vector3f& aPos);
	void SetAmbientLight(float aIntensity);
	void BeginShadowRendering();
	void EndShadowRendering();
	void EndOfRenderPassReset();

	void SetResourcesOnSlot();
	void ClearAllResources();
	

	DirectionalLight& GetDirectionalLight() { return *myDirectionalLight; }

#ifdef _EDITOR
	LightBufferData& GetLightDataRef() { return myLightData; }
#endif
private:
	LightBufferData myLightData;
	ComPtr<ID3D11Buffer> myLightBuffer;

	RenderTarget myPointLightsRenderTarget;
	RenderTarget myDirectionalLightRenderTarget;

	SharedMesh* myLightSphereMesh;

	//PointLight
	std::vector<PointLight> myPointLights;
	const PixelShader* myBoundingPixelShader;

	//DirectionalLight
	DirectionalLight* myDirectionalLight = nullptr;
	Camera* myLastUsedCamera = nullptr;

};