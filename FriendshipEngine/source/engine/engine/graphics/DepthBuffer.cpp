#include "pch.h"
#include "DepthBuffer.h"
#include "GraphicsEngine.h"
#include "../math/Vector.h"

DepthBuffer DepthBuffer::Create(const Vector2i& aSize)
{
	auto& device = GraphicsEngine::GetInstance()->DX().GetDevice();//GlobalStuff::GetInstance().GetGraphicsEngine()->GetDevice();

	DepthBuffer depthBufferResult;

	HRESULT result;
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = aSize.x;
	desc.Height = aSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.MiscFlags = 0;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	result = device->CreateTexture2D(&desc, nullptr, &depthBufferResult.myTexture);
	assert(SUCCEEDED(result));

	ID3D11DepthStencilView* DSV;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};

	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	result = device->CreateDepthStencilView(depthBufferResult.myTexture, &dsvDesc, &DSV);
	assert(SUCCEEDED(result));

	depthBufferResult.myDepth = DSV;
	DSV->Release();

	ID3D11ShaderResourceView* SRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = UINT_MAX;
	result = device->CreateShaderResourceView(depthBufferResult.myTexture, &srvDesc, &SRV);
	assert(SUCCEEDED(result));
	depthBufferResult.SRV = SRV;
	SRV->Release();

	depthBufferResult.myViewport = {
			0,
			0,
			static_cast<float>(aSize.x),
			static_cast<float>(aSize.y),
			0,
			1
	};
	
	depthBufferResult.CreateStagingTexture(&srvDesc);

	return depthBufferResult;
}

void DepthBuffer::Clear(float aClearDepthValue, unsigned char aClearStencilValue)
{
	auto context = GraphicsEngine::GetInstance()->DX().GetContext();

	context->ClearDepthStencilView(myDepth.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, aClearDepthValue, aClearStencilValue);
}

void DepthBuffer::SetAsActiveTarget()
{
	auto context = GraphicsEngine::GetInstance()->DX().GetContext();

	context->OMSetRenderTargets(0, nullptr, GetDepthStencilView());
	context->RSSetViewports(1, &myViewport);
}

Vector2i DepthBuffer::GetSize() const
{
	Vector2i size;
	size.x = (int)myViewport.Width;
	size.y = (int)myViewport.Height;
	return size;
}
