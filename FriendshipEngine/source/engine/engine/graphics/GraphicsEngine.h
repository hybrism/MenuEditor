#pragma once

#include <wrl/client.h>
#include <string>
#include "../math/Vector.h"
#include "DepthBuffer.h"
#include "GBuffer.h"
#include "model/MeshDrawer.h"
#include "renderer/DeferredRenderer.h"
#include "renderer/ForwardRenderer.h"
#include "renderer/DebugRenderer.h"
#include "renderer/SpriteRenderer.h"
#include "RenderDefines.h"
#include <array>
#include "DxData.h"

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
class GBuffer;
class LightManager;
class PostProcess;

class GraphicsEngine
{
private:
	struct GERenderers
	{
		ForwardRenderer forwardRenderer;
		DeferredRenderer deferredRenderer;
		DebugRenderer debugRenderer;
		SpriteRenderer spriteRenderer;
		MeshDrawer meshDrawer;

		GERenderers() : debugRenderer(), spriteRenderer(), meshDrawer(), forwardRenderer(meshDrawer), deferredRenderer(meshDrawer)
		{

		}
	};

	struct GERenderStateCollection
	{
		std::array<ComPtr<ID3D11DepthStencilState>, static_cast<int>(DepthStencilState::Count)> myDepthStencilStates;
		std::array<ComPtr<ID3D11RasterizerState>, static_cast<int>(RasterizerState::Count)> myRasterizerStates;
		std::array<ComPtr<ID3D11BlendState>, (int)BlendState::Count> myBlendStates;
		std::array<std::array<ComPtr<ID3D11SamplerState>, (int)SamplerAddressMode::Count>, (int)SamplerFilter::Count> mySamplerStates;
	};
	friend class Engine;
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
	inline ComPtr<ID3D11Buffer>& GetObjectBuffer() { return myObjectBuffer; }
	inline ComPtr<ID3D11Buffer>& GetInputBuffer() { return myInputBuffer; }

	~GraphicsEngine();
	bool Initialize(int width, int height, HWND windowHandle);
	void BeginFrame();
	void EndFrame();

	ComPtr<ID3D11RenderTargetView>& GetBackBuffer() { return myBackBufferRenderTarget.RTV; }
	ComPtr<ID3D11ShaderResourceView>& GetBackBufferSRV() { return myBackBufferRenderTarget.SRV; }
	RenderTarget& GetBackBufferRenderTarget();
	DepthBuffer& GetDepthBuffer() { return myDxData.GetDepthBuffer(); }

	Vector2i& GetInternalResolution() { return myInternalResolution; }

	void SetFullscreen(
		bool aIsFullscreen,
		LightManager* aLightManager,
		PostProcess* aPostProcess
	);

	void SetResolution(
		const Vector2i& aResolution,
		LightManager* aLightManager,
		PostProcess* aPostProcess
	);

	void SetResolution(
		const Vector2i& aResolution,
		bool aIsFullscreen,
		LightManager* aLightManager,
		PostProcess* aPostProcess
	);
	void SetInternalResolution(const Vector2i& aResolution);
	bool SetWindowDimensions(const Vector2i& aResolution);

	Camera* GetCamera() { return myCurrentCamera; }
	Camera* GetViewCamera() { return myViewCamera; }
	Camera const* GetCamera() const { return myCurrentCamera; }
	void ChangeCurrentCamera(Camera* aCamera);
	void ResetToViewCamera();

	//Used to stop CameraSystem from updating the player while in free camera mode
	void SetUseOfFreeCamera(const bool& aBool);
	bool IsFreeCameraInUse() const { return myIsUsingFreeCamera; }

	MeshDrawer& GetMeshDrawer() { return myRenderers.meshDrawer; }

	GBuffer& GetGBuffer() { return myGBuffer; }

	void SetRasterizerState(const RasterizerState& aRasterizerState);
	void SetDepthStencilState(const DepthStencilState& aDepthStencilState);
	void SetBlendState(const BlendState& aBlendState);
	void SetRenderState(const RenderState& aRenderState);
	void SetSamplerState(const SamplerFilter& aSamplerFilter, const SamplerAddressMode& aSamplerAddressMode);
	RenderState GetRenderState() const { return myRenderState; }

	void SetCubemap(const std::string& aCubemapLocation);
	void SetAsActiveTarget(const int& aNumViews, ComPtr<ID3D11RenderTargetView>* aViews, DepthBuffer* aDepth, const D3D11_VIEWPORT* aViewport = nullptr);
	void SetBackBufferAsActiveTarget(DepthBuffer* aDepth);
	void SetBackBufferAsActiveTarget();
	void IncrementRenderMode();
	void IncrementDrawCalls()
	{
		myDrawCalls++;
	}
	// TODO: Remove this, due to the current editor structure, it is updated before the game loop which in turn causes the draw calls to be updated after editor and reset at the start of the next frame
	void ResetDrawCalls() { myDrawCalls = 0; }
	unsigned int GetDrawCalls() const { return myDrawCalls; }

	unsigned int GetRenderMode() const { return myRenderMode; }

	void SetRawBackBufferAsRenderTarget();
	void SetRawBackBufferAsRenderTarget(DepthBuffer* aDepth);
	ComPtr<ID3D11RenderTargetView> GetRawBackBufferRenderTarget() { return myDxData.GetBackBuffer(); }

	DeferredRenderer& GetDeferredRenderer() { return myRenderers.deferredRenderer; }
	ForwardRenderer& GetForwardRenderer() { return myRenderers.forwardRenderer; }
	DebugRenderer& GetDebugRenderer() { return myRenderers.debugRenderer; }
	SpriteRenderer& GetSpriteRenderer() { return myRenderers.spriteRenderer; }

	DxData& DX() { return myDxData; }

	void SetClearColor(const Vector3f& aColor)
	{
		myClearColor.x = aColor.x;
		myClearColor.y = aColor.y;
		myClearColor.z = aColor.z;
	}
	void SetClearColor(float aX, float aY, float aZ)
	{
		myClearColor.x = aX;
		myClearColor.y = aY;
		myClearColor.z = aZ;
	}
	Vector3f GetClearColor() const { return { myClearColor.x, myClearColor.y, myClearColor.z }; }

	float GetDepthFadeK() const { return myDepthFadeK; }
	void SetDepthFadeK(const float& aDepthFadeK) { myDepthFadeK = aDepthFadeK; }
#ifdef _EDITOR
	float& GetDepthFadeKRef() { return myDepthFadeK; }
#endif
private:
	GraphicsEngine();

	float GetHeight(float x, float y);

	bool CreateBlendStates();
	bool CreateDepthStencilStates();
	bool CreateRasterizerStates();
	bool CreateSamplerStates();
	void UpdateFrameBuffer();
	void SetInternalFullscreen(bool aIsFullscreen);
	

	ComPtr<ID3D11Buffer> myFrameBuffer;
	ComPtr<ID3D11Buffer> myObjectBuffer;
	ComPtr<ID3D11Buffer> myCameraBuffer;
	ComPtr<ID3D11Buffer> myInputBuffer;

	ComPtr<ID3D11ShaderResourceView> myCubemap;

	GBuffer myGBuffer;
	GERenderers myRenderers;
	GERenderStateCollection myRenderStates;

	Vector2i myBackBufferDimensions;
	Vector2i myInternalResolution;
	Vector2i myTargetResolution;
	
	RenderTarget myBackBufferRenderTarget;

	Camera* myViewCamera;
	Camera* myCurrentCamera;
	
	DxData myDxData;

	Vector4f myClearColor;

	unsigned int myDrawCalls = 0;
	unsigned int myNumMips = 0;
	RenderState myRenderState;
	unsigned int myRenderMode = 0;
	float myDepthFadeK = 1400;
	bool myIsUsingFreeCamera = false;

	static GraphicsEngine* myInstance;
};
