#pragma once
#include <wrl/client.h>
#include "RenderTarget.h"
#include "DepthBuffer.h"
#include "../math/Vector.h"
#include "../CoreTypes.h"

using Microsoft::WRL::ComPtr;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;

struct D3D11_VIEWPORT;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11DepthStencilView;
struct ID3D11SamplerState;
struct ID3D11RasterizerState;
struct ID3D11Buffer;
struct ID3D11BlendState;

enum class SwapChainState
{
	VSync = 0,
	NoVSync = 8UL
};

enum class ShaderType
{
	VS,
	PS,
	GS,
	HS,
	DS,
	CS
};

class DxData
{
public:
	DxData();
	~DxData();

	inline ID3D11DeviceContext* GetContext() { return myContext.Get(); }
	inline ComPtr<ID3D11Device>& GetDevice() { return myDevice; }

	bool Init(const int aWidth, const int aHeight, HWND aWND);
	void EndFrame();

	bool SetResolution(HWND aWND);
	void SetSwapChainState(const SwapChainState& aState) { mySwapChainState = aState; }
	void SetViewport();

	ComPtr<ID3D11RenderTargetView>& GetBackBuffer() { return myBackBuffer; }
	ComPtr<IDXGISwapChain>& GetSwapChain() { return mySwapChain; }
	DepthBuffer& GetDepthBuffer() { return myDepthBuffer; }
	D3D11_VIEWPORT& GetViewport() { return myViewport; }

	void SetWindowDimensions(Vector2i aResolution) { myWindowDimensions = aResolution; }
	Vector2i GetWindowDimensions() const { return myWindowDimensions; }
	Vector2i GetViewportDimensions() const { return myViewportDimensions; }

	void UnsetSRV(ShaderType aType, uint32 aStartSlot, uint32 aNumOfViews);
	void UnsetUAV(uint32 aStartSlot, uint32 aNumOfViews);
private:
	//RenderTarget myBackBuffer;
	DepthBuffer myDepthBuffer;

	Vector2i myWindowDimensions;
	Vector2i myViewportDimensions;

	D3D11_VIEWPORT myViewport = {};
	ComPtr<ID3D11RenderTargetView> myBackBuffer;
	ComPtr<ID3D11Device> myDevice;
	ComPtr<ID3D11DeviceContext> myContext;
	ComPtr<IDXGISwapChain> mySwapChain;

	SwapChainState mySwapChainState = SwapChainState::VSync;
};