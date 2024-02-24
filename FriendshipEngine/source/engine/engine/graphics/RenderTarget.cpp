#include "pch.h"
#include "RenderTarget.h"
#include "GraphicsEngine.h"
#include <assert.h>

RenderTarget::RenderTarget() : RTV(nullptr), SRV(nullptr), myViewport({}) {}
RenderTarget::~RenderTarget() = default;

RenderTarget RenderTarget::Create(const D3D11_TEXTURE2D_DESC& aDesc)
{
	HRESULT result;
	ID3D11Texture2D* texture;
	result = GraphicsEngine::GetInstance()->GetDevice()->CreateTexture2D(&aDesc, nullptr, &texture);
	assert(SUCCEEDED(result));

	RenderTarget textureResult = Create(texture);

	ID3D11ShaderResourceView* SRV;
	result = GraphicsEngine::GetInstance()->GetDevice()->CreateShaderResourceView(texture, nullptr, &SRV);
	assert(SUCCEEDED(result));
	textureResult.SRV = SRV;
	SRV->Release();

	return textureResult;
}

RenderTarget RenderTarget::Create(const Vector2i& aSize, DXGI_FORMAT aFormat)
{
	HRESULT result;
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = (unsigned int)aSize.x;
	desc.Height = (unsigned int)aSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = aFormat;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* texture;
	result = GraphicsEngine::GetInstance()->GetDevice()->CreateTexture2D(&desc, nullptr, &texture);
	assert(SUCCEEDED(result));

	RenderTarget textureResult = Create(texture);

	ID3D11ShaderResourceView* SRV;
	result = GraphicsEngine::GetInstance()->GetDevice()->CreateShaderResourceView(texture, nullptr, &SRV);
	assert(SUCCEEDED(result));
	textureResult.SRV = SRV;
	SRV->Release();

	return textureResult;
}

RenderTarget RenderTarget::Create(ID3D11Texture2D* aTexture)

{
	HRESULT result;

	ID3D11RenderTargetView* RTV;
	result = GraphicsEngine::GetInstance()->GetDevice()->CreateRenderTargetView(
		aTexture,
		nullptr,
		&RTV);
	assert(SUCCEEDED(result));

	RenderTarget textureResult;

	if (aTexture)
	{
		textureResult.myTexture = aTexture;
		D3D11_TEXTURE2D_DESC desc;
		aTexture->GetDesc(&desc);
		textureResult.myViewport = {
				0,
				0,
				static_cast<float>(desc.Width),
				static_cast<float>(desc.Height),
				0,
				1
		};
	}

	textureResult.RTV = RTV;
	RTV->Release();

	textureResult.CreateStagingTexture();

	return textureResult;
}

void RenderTarget::Clear(const Vector4f& aColor) const
{
	GraphicsEngine::GetInstance()->GetContext()->ClearRenderTargetView(RTV.Get(), &aColor.x);
}

void RenderTarget::SetAsTarget(DepthBuffer* aDepthBuffer)
{
	void* depthBuffer = aDepthBuffer ? aDepthBuffer->GetDepthStencilView() : nullptr;

	GraphicsEngine::GetInstance()->GetContext()->OMSetRenderTargets(1, RTV.GetAddressOf(), (ID3D11DepthStencilView*)depthBuffer);
	GraphicsEngine::GetInstance()->GetContext()->RSSetViewports(1, &myViewport);
}
