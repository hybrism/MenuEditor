#pragma once
#include <wrl/client.h>
#include "GraphicsEngine.h"
#include "GraphicsData.h"
#include "RenderTarget.h"

using Microsoft::WRL::ComPtr;

#define USE_POST_PROCESSING 1

class PostProcess
{
	struct RenderTargetWrapper
	{
		RenderTarget target;
		int downscaleLevel = 0;
	};

public:
	struct VignetteData
	{
		float vignetteInner = 0.2f;
		float vignetteOuter = 3.2f;
		float vignetteStrength = 0.0f;
		float vignetteCurvature = 0.3f;
	};

	PostProcess();
	~PostProcess();

	void Init();
	void Render();

	void FirstFrame();

	void SetBloomThreshold(float aThreshold) { myBufferData.alphaBlendLevel = aThreshold; }
	void SetSaturation(float aSaturation) { myBufferData.saturation = aSaturation; }
	void SetExposure(float aExposure) { myBufferData.exposure = aExposure; }
	void SetTint(const Vector3f& aTint) { myBufferData.tint = aTint; }
	void SetContrast(const float& aContrast) { myBufferData.contrast = aContrast; }
	void SetBlackPoint(const Vector3f& aBlackPoint) { myBufferData.blackPoint = aBlackPoint; }
	void SetVignetteData(const VignetteData& aVignetteData)
	{
		myBufferData.vignetteInner = aVignetteData.vignetteInner;
		myBufferData.vignetteOuter = aVignetteData.vignetteOuter;
		myBufferData.vignetteStrength = aVignetteData.vignetteStrength;
		myBufferData.vignetteCurvature = aVignetteData.vignetteCurvature;
	}
	void SetVignetteStrength(float aStrength) { myBufferData.vignetteStrength = aStrength; }
	PostProcessBufferData& GetBufferData() { return myBufferData; }
private:
	void ClearShaderResources();

	PostProcessBufferData myBufferData;
	RenderTarget myRenderTargets[2];
	std::vector<RenderTargetWrapper> myDownScaledRenderTargets;
};