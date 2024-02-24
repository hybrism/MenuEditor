#pragma once
#include <wrl/client.h>
#include "GraphicsEngine.h"
#include "GraphicsData.h"
#include "RenderTarget.h"

using Microsoft::WRL::ComPtr;

#define USE_POST_PROCESSING 0

class PostProcess
{
	struct RenderTargetWrapper
	{
		RenderTarget target;
		int downscaleLevel = 0;
	};
public:
	PostProcess();
	~PostProcess();

	void Init();
	void Render();

	void FirstFrame();

	void SetBloomThreshold(float aThreshold) { myBufferData.alphaBlendLevel = aThreshold; }
	void SetSaturation(float aSaturation) { myBufferData.saturation = aSaturation; }
	void SetExposure(float aExposure) { myBufferData.exposure = aExposure; }
	void SetTint(const Vector3f& aTint) { myBufferData.tint = aTint; }
	void SetContrast(const Vector3f& aContrast) { myBufferData.contrast = aContrast; }
	void SetBlackPoint(const Vector3f& aBlackPoint) { myBufferData.blackPoint = aBlackPoint; }
private:
	void ClearShaderResources();

	PostProcessBufferData myBufferData;
	RenderTarget myRenderTargets[2];
	std::vector<RenderTargetWrapper> myDownScaledRenderTargets;
};