#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include "../math/VectorFwd.h"
#include "StagingTexture.h"

using Microsoft::WRL::ComPtr;

struct ID3D11DepthStencilView;
struct ID3D11Texture2D;
struct D3D11_VIEWPORT;

class DepthBuffer : public StagingTexture
{
public:
	static DepthBuffer Create(const Vector2i& aSize);

	void Clear(float aClearDepthValue = 1.0f, unsigned char aClearStencilValue = 0);
	void SetAsActiveTarget();

	Vector2i GetSize() const;
	inline ID3D11DepthStencilView* GetDepthStencilView() { return myDepth.Get(); };
private:
	D3D11_VIEWPORT myViewport = {};
	ComPtr<ID3D11DepthStencilView> myDepth = 0;
};