#pragma once
#include <d3d11.h>
#include "StagingTexture.h"
#include "../math/VectorFwd.h"

using Microsoft::WRL::ComPtr;
class DepthBuffer;

struct RenderTarget : public StagingTexture
{
public:
	RenderTarget();
	~RenderTarget();

	static RenderTarget Create(const D3D11_TEXTURE2D_DESC& aDesc);
	static RenderTarget Create(const Vector2i& aSize, DXGI_FORMAT aFormat);
	static RenderTarget Create(ID3D11Texture2D* aTexture);

	void Clear(const Vector3f& aColor) const;
	void SetAsTarget(DepthBuffer* aDepthBuffer = nullptr);

	D3D11_VIEWPORT myViewport = { 0 };
	ComPtr<ID3D11RenderTargetView> RTV;
};