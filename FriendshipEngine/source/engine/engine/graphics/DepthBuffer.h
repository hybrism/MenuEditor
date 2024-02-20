#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include "../math/VectorFwd.h"

using Microsoft::WRL::ComPtr;

struct ID3D11DepthStencilView;
struct ID3D11Texture2D;
struct D3D11_VIEWPORT;

class DepthBuffer
{
public:
	static DepthBuffer Create(const Vector2i& aSize);
	static DepthBuffer* CreateNewPointer(const Vector2i& aSize);

	void Clear(float aClearDepthValue = 1.0f, unsigned char aClearStencilValue = 0);
	inline ID3D11DepthStencilView* GetDepthStencilView() { return myDepth.Get(); };

	void SetAsActiveTarget();

	ComPtr<ID3D11ShaderResourceView> mySRV;
private:
	ComPtr<ID3D11DepthStencilView> myDepth = 0;
	D3D11_VIEWPORT myViewport = {};
};