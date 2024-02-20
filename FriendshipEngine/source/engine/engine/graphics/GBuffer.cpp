#include "pch.h"
#include "GBuffer.h"
#include "DepthBuffer.h"

#include "GraphicsEngine.h"
#include "../Engine.h"

GBuffer::GBuffer() { }
GBuffer::~GBuffer() { }

GBuffer GBuffer::Create(const Vector2i& aSize)
{
	GBuffer buffer;
	auto* graphicsEngine = GraphicsEngine::GetInstance();
	auto& device = graphicsEngine->GetDevice();

	HRESULT result;
	std::array<DXGI_FORMAT, (int)GBufferTexture::Count> textureFormats =
	{
		DXGI_FORMAT_R32G32B32A32_FLOAT,// World Position
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,// Albedo
		DXGI_FORMAT_R16G16B16A16_SNORM,// Normal,
		DXGI_FORMAT_R16G16B16A16_SNORM,// VertexNormal,
		DXGI_FORMAT_R8G8B8A8_UNORM,// Material
		DXGI_FORMAT_R8G8B8A8_UNORM// Effects, add channels more if needed
	};

	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = static_cast<unsigned int>(aSize.x);
	desc.Height = static_cast<unsigned int>(aSize.y);
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	for (unsigned int idx = 0; idx < (int)GBufferTexture::Count; idx++)
	{
		desc.Format = textureFormats[idx];

		result = device->CreateTexture2D(
			&desc,
			nullptr,
			&buffer.myTextures[idx]
		);
		assert(SUCCEEDED(result));

		result = device->CreateRenderTargetView(
			buffer.myTextures[idx].Get(),
			nullptr,
			buffer.myRTVs[idx].GetAddressOf()
		);
		assert(SUCCEEDED(result));

		result = device->CreateShaderResourceView(
			buffer.myTextures[idx].Get(),
			nullptr,
			buffer.mySRVs[idx].GetAddressOf()
		);
		assert(SUCCEEDED(result));
	}

	buffer.myViewport = std::make_shared<D3D11_VIEWPORT>(
		D3D11_VIEWPORT{
			0,
			0,
			static_cast<float>(desc.Width),
			static_cast<float>(desc.Height),
			0,
			1
		}
	);

	return buffer;
}

void GBuffer::ClearTextures()
{
	auto* graphicsEngine = GraphicsEngine::GetInstance();
	auto* context = graphicsEngine->GetContext();

	Vector4f clearColor = { 0,0,0,0 };
	for (unsigned int idx = 0; idx < (int)GBufferTexture::Count; idx++)
	{
		context->ClearRenderTargetView(myRTVs[idx].Get(), &clearColor.x);
	}
}

void GBuffer::SetAsActiveTarget()
{
	auto* ge = GraphicsEngine::GetInstance();
	ge->SetAsActiveTarget((int)GBufferTexture::Count, myRTVs.data(), &ge->GetDepthBuffer(), myViewport.get());
}

void GBuffer::SetAsResourceOnSlot(GBufferTexture aTexture, unsigned int aSlot)
{
	auto* graphicsEngine = GraphicsEngine::GetInstance();
	auto* context = graphicsEngine->GetContext();

	context->PSSetShaderResources(aSlot, 1, mySRVs[(int)aTexture].GetAddressOf());
}

void GBuffer::SetAllAsResources(const unsigned int& aStartSlot)
{
	auto* context = GraphicsEngine::GetInstance()->GetContext();

	context->PSSetShaderResources(aStartSlot, (unsigned int)GBufferTexture::Count, mySRVs[0].GetAddressOf());
}

void GBuffer::ClearAllResources(const unsigned int& aStartSlot)
{
	auto* context = GraphicsEngine::GetInstance()->GetContext();

	ID3D11ShaderResourceView* nullSRV = nullptr;
	for (unsigned int idx = 0; idx < (unsigned int)GBufferTexture::Count; idx++)
	{
		context->PSSetShaderResources(aStartSlot + idx, 1, &nullSRV);
	}
}
