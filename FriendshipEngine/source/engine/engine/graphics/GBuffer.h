#pragma once

#include "../math/Vector.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <array>
#include <memory>
#include "RenderTarget.h"

using Microsoft::WRL::ComPtr;

class DepthBuffer;

enum class GBufferTexture
{
	WorldPosition, // Wastes space and can be reconstructed from depth and screen pos, but we store it for simplicity
	Albedo,
	Normal, // stored as 0.5f + 0.5f*normal
	VertexNormal,
	Material,
	AmbientOcclusionAndCustom, // Ambient occlusion in R, rest is unused
	Count
};

class GBuffer
{
public:
	GBuffer();
	~GBuffer();

	static GBuffer Create(const Vector2i& aSize);

	void ClearTextures();
	void SetAsActiveTarget();
	void SetAsResourceOnSlot(GBufferTexture aTexture, unsigned int aSlot);
	void SetAllAsResources(const unsigned int& aStartSlot);
	void ClearAllResources(const unsigned int& aStartSlot);
#ifdef _DEBUG
	void CopyToStaging();
#endif
public: // temp
	std::array<RenderTarget, (int)GBufferTexture::Count> myRenderTargets;
	std::array<ComPtr<ID3D11Texture2D>, (int)GBufferTexture::Count> myTextures;
	std::array<ComPtr<ID3D11RenderTargetView>, (int)GBufferTexture::Count> myRTVs;
	std::array<ComPtr<ID3D11ShaderResourceView>, (int)GBufferTexture::Count> mySRVs;
	std::shared_ptr<const D3D11_VIEWPORT> myViewport;
};