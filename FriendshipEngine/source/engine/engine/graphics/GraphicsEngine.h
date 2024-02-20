#pragma once

#include <wrl/client.h>
#include <string>
#include "../math/Vector.h"
#include "DepthBuffer.h"
#include "GBuffer.h"
#include "model/MeshDrawer.h"
#include "renderer/DeferredRenderer.h"
#include "renderer/ForwardRenderer.h"
#include "RenderDefines.h"
#include <array>

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

struct Vertex;
struct RenderTarget;
class Camera;
class TextService;
class SpriteDrawer;
class GBuffer;
class DirectionalLightManager;

class GraphicsEngine
{
	friend class Text;
	friend class TextService;
public:
	inline static GraphicsEngine* GetInstance() 
	{ 
		if (myInstance == nullptr)
		{
			myInstance = new GraphicsEngine();
		}
		return myInstance; 
	}

	inline static void DestroyInstance()
	{
		if (myInstance != nullptr)
		{
			delete myInstance;
			myInstance = nullptr;
		}
	}

	inline ID3D11DeviceContext* GetContext() { return myContext.Get(); }
	inline ComPtr<ID3D11Buffer>& GetObjectBuffer() { return myObjectBuffer; }
	inline ComPtr<ID3D11Buffer>& GetPointLightBuffer() { return myPointLightBuffer; }
	inline ComPtr<ID3D11Device>& GetDevice() { return myDevice; }
	inline ComPtr<ID3D11Buffer>& GetInputBuffer() { return myInputBuffer; }

	~GraphicsEngine();
	bool Initialize(int width, int height, HWND windowHandle);
	void BeginFrame();
	void EndFrame();
	void SetClearColor(float r, float g, float b)
	{
		myClearColor.x = r;
		myClearColor.y = g;
		myClearColor.z = b;
	}

	Vector4f GetClearColor() const { return myClearColor; }

	void PrepareForSpriteRender();
	void ResetToDefault();

	const Vector2<int>& GetWindowDimensions() const { return myWindowDimensions; }
	const Vector2<int>& GetViewportDimensions() const { return myViewportDimensions; }
	const Vector2<float>& GetBackTexturesize() const { return myBackBufferTextureSize; }
	const D3D11_VIEWPORT& GetViewPort() const {return *myViewport;}
	ComPtr<ID3D11RenderTargetView>& GetBackBuffer();
	ComPtr<ID3D11ShaderResourceView>& GetBackBufferSRV();
	DepthBuffer& GetDepthBuffer() { return myDepthBuffer; }
	bool SetResolution(const Vector2<int>& aResolution);

	Camera* GetCamera() { return myCurrentCamera; }
	Camera const* GetCamera() const { return myCurrentCamera; }
	void ChangeCurrentCamera(Camera* aCamera);
	void ResetToPrimaryCamera();

	SpriteDrawer& GetSpriteDrawer() { return *mySpriteDrawer; }
	MeshDrawer& GetMeshDrawer() { return myMeshDrawer; }

	GBuffer& GetGBuffer() { return myGBuffer; }

	void SetRasterizerState(const RasterizerState& aRasterizerState);
	void SetDepthStencilState(const DepthStencilState& aDepthStencilState);
	void SetBlendState(const BlendState& aBlendState);
	void SetRenderState(const RenderState& aRenderState);
	RenderState GetRenderState() const { return myRenderState; }

	void SetCubemap(const std::string& aCubemapLocation);
	void SetAsActiveTarget(const int& aNumViews, ComPtr<ID3D11RenderTargetView>* aViews, DepthBuffer* aDepth, const D3D11_VIEWPORT* aViewport = nullptr);
	void SetBackBufferAsActiveTarget(DepthBuffer* aDepth);
	void SetBackBufferAsActiveTarget();
	void IncrementRenderMode();
	void IncrementDrawCalls()
	{
#ifdef _DEBUG
		myDrawCalls++;
#endif
	}
	// TODO: Remove this, due to the current editor structure, it is updated before the game loop which in turn causes the draw calls to be updated after editor and reset at the start of the next frame

#ifdef _DEBUG
	void ResetDrawCalls() { myDrawCalls = 0; }
	unsigned int GetDrawCalls() const { return myDrawCalls; }
#endif
	unsigned int GetRenderMode() const { return myRenderMode; }
	DirectionalLightManager* GetDirectionalLightManager() { return myDirectionalLightManager; }

	void SetRawBackBufferAsRenderTarget();
	void SetRawBackBufferAsRenderTarget(DepthBuffer* aDepth);
	ComPtr<ID3D11RenderTargetView> GetRawBackBufferRenderTarget() { return myBackBuffer; }

	DeferredRenderer& GetDeferredRenderer() { return myDeferredRenderer; }
	ForwardRenderer& GetForwardRenderer() { return myForwardRenderer; }
private:
	GraphicsEngine();

	float GetHeight(float x, float y);

	bool CreateBlendStates();
	bool CreateDepthStencilStates();
	bool CreateRasterizerStates();
	void CalculateBinormalTangent(Vertex* vertices, const size_t& vertexCount);
	void UpdateFrameBuffer();

	TextService& GetTextService() const { return *myTextService; }

	std::array<ComPtr<ID3D11DepthStencilState>, static_cast<int>(DepthStencilState::Count)> myDepthStencilStates;
	std::array<ComPtr<ID3D11RasterizerState>, static_cast<int>(RasterizerState::Count)> myRasterizerStates;
	std::array<ComPtr<ID3D11BlendState>, (int)BlendState::Count> myBlendStates;


	ComPtr<ID3D11Device> myDevice;
	ComPtr<ID3D11DeviceContext> myContext;
	ComPtr<IDXGISwapChain> mySwapChain;
	ComPtr<ID3D11Buffer> myFrameBuffer;
	ComPtr<ID3D11Buffer> myObjectBuffer;
	ComPtr<ID3D11Buffer> myPointLightBuffer;
	ComPtr<ID3D11Buffer> myCameraBuffer;
	ComPtr<ID3D11Buffer> myInputBuffer;
	ComPtr<ID3D11SamplerState> mySamplerState;
	ComPtr<ID3D11SamplerState> my2dSamplerState; // TODO: use same sampler??? in 2d Mips LOD is not working
	ComPtr<ID3D11SamplerState> myClampSamplerState;
	ComPtr<ID3D11ShaderResourceView> myCubemap;
	ComPtr<ID3D11RasterizerState> myFrontFaceCullingRasterizerState;
	ComPtr<ID3D11RenderTargetView> myBackBuffer = nullptr;

	D3D11_VIEWPORT* myViewport = nullptr;
	GBuffer myGBuffer;
	DepthBuffer myDepthBuffer;
	ForwardRenderer myForwardRenderer;
	DeferredRenderer myDeferredRenderer;

	RenderTarget* myBackBufferRenderTarget;

	ComPtr<ID3D11BlendState> myInactivatedBlendState;
	ComPtr<ID3D11BlendState> myAdditiveBlendState;
	ComPtr<ID3D11BlendState> myAlphaBlendState;
	ID3D11DepthStencilState* myDepthStencilState;
	ID3D11DepthStencilState* myAdditiveStencilState;


	Camera* myViewCamera;
	Camera* myCurrentCamera;
	TextService* myTextService;
	SpriteDrawer* mySpriteDrawer;
	MeshDrawer myMeshDrawer;
	DirectionalLightManager* myDirectionalLightManager;

	Vector2i myWindowDimensions;
	Vector2i myViewportDimensions;
	Vector4f myClearColor;

#ifdef _DEBUG
	unsigned int myDrawCalls = 0;
#endif
	unsigned int myNumMips = 0;
	static GraphicsEngine* myInstance;
	RenderState myRenderState;
	unsigned int myRenderMode = 0;

	Vector2f myBackBufferTextureSize;
};
