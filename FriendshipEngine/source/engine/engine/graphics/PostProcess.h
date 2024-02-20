#pragma once
#include <wrl/client.h>
#include "GraphicsEngine.h"
#include "GraphicsData.h"

using Microsoft::WRL::ComPtr;
struct RenderTarget;

#define USE_POST_PROCESSING 1

enum class eRenderStates
{
	eAdditive = 0,
	eAlpha,
	eInactive,
};
class PostProcess
{
public:
	PostProcess();
	~PostProcess();

	void Init();
	void Update();
	void Render();

	void FirstFrame();

	bool CreateBlendStates();
	void UseAdditiveState(eRenderStates aState);

	void UpdatePostProcessBuffer();

	void ImguiAdjustBloom();


private:
	void ClearShaderResources();

	std::vector<RenderTarget*> myDownScaledRenderTargets;
	PostProcessBufferData* myBufferData;
	RenderTarget* myRenderTarget;
	RenderTarget* myRenderTargetEffect;

	ComPtr<ID3D11Buffer> myFrameBuffer;
	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;
	ComPtr<ID3D11Buffer> myPostProcessBuffer;

	ComPtr<ID3D11InputLayout> myInputLayout;
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
	ComPtr<ID3D11PixelShader> myToneMapPixelShader;
	ComPtr<ID3D11PixelShader> myVingettePixelShader;
	ComPtr<ID3D11Buffer> myObjectBuffer;


	ComPtr<ID3D11BlendState> myInactivatedBlendState;
	ComPtr<ID3D11BlendState> myAdditiveBlendState;
	ComPtr<ID3D11BlendState> myAlphaBlendState;
	ID3D11DepthStencilState* myDepthStencilState;
	ID3D11DepthStencilState* myAdditiveStencilState;


	//IMGUI BUFFER TINGELING
	float myAlphaBlendLevel = 0.405f;
	float mySaturation = 1.1f;
	float myExposure = 0.f;
	float myContrast[3] = { 1.f, 1.f, 1.f };
	float myTint[3] = { 1.f, 1.f,1.f };
	float myBlackPoint[3] = { 0.f , 0.f, 0.f };
};