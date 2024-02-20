#include "pch.h"
#include "DepthBuffer.h"
#include "GraphicsEngine.h"
#include "../math/Vector.h"

DepthBuffer DepthBuffer::Create(const Vector2i& aSize)
{
	auto& device = GraphicsEngine::GetInstance()->GetDevice();//GlobalStuff::GetInstance().GetGraphicsEngine()->GetDevice();

	HRESULT result;
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = aSize.x;
	desc.Height = aSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* texture;
	result = device->CreateTexture2D(&desc, nullptr, &texture);
	assert(SUCCEEDED(result));

	ID3D11DepthStencilView* DSV;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};

	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	result = device->CreateDepthStencilView(texture, &dsvDesc, &DSV);
	assert(SUCCEEDED(result));

	DepthBuffer textureResult;
	textureResult.myDepth = DSV;
	DSV->Release();

	ID3D11ShaderResourceView* SRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc{};
	srDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = UINT_MAX;
	result = device->CreateShaderResourceView(texture, &srDesc, &SRV);
	assert(SUCCEEDED(result));
	textureResult.mySRV = SRV;
	SRV->Release();

	textureResult.myViewport = {
			0,
			0,
			static_cast<float>(aSize.x),
			static_cast<float>(aSize.y),
			0,
			1
	};
	return textureResult;
}

void DepthBuffer::Clear(float aClearDepthValue, unsigned char aClearStencilValue)
{
	auto context = GraphicsEngine::GetInstance()->GetContext();

    context->ClearDepthStencilView(myDepth.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, aClearDepthValue, aClearStencilValue);
}

void DepthBuffer::SetAsActiveTarget()
{
	auto context = GraphicsEngine::GetInstance()->GetContext();

    context->OMSetRenderTargets(0, nullptr, GetDepthStencilView());
    context->RSSetViewports(1, &myViewport);
}
