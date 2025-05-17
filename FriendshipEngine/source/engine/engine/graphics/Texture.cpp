#include "pch.h"
#include "Texture.h"
#include "GraphicsEngine.h"
#include "RenderDefines.h"
#include <d3d11.h>

Texture::Texture(ComPtr<ID3D11ShaderResourceView> aShaderResourceView)
{
	myShaderResourceView = aShaderResourceView;
	myWidth = 0;
	myHeight = 0;
	myRgbaPixels = nullptr;
	myTexture = nullptr;
}

Texture::Texture(ComPtr<ID3D11ShaderResourceView> aShaderResourceView, const unsigned int& aWidth, const unsigned int& aHeight)
{
	myShaderResourceView = aShaderResourceView;
	myWidth = aWidth;
	myHeight = aHeight;
	myRgbaPixels = nullptr;
	myTexture = nullptr;
}

Texture::Texture(ComPtr<ID3D11ShaderResourceView> aShaderResourceView, const unsigned int& aWidth, const unsigned int& aHeight, ComPtr<ID3D11Texture2D> aTexture)
{
	myShaderResourceView = aShaderResourceView;
	myWidth = aWidth;
	myHeight = aHeight;
	myTexture = aTexture;
	myRgbaPixels = nullptr;
}

Texture::Texture(
	ComPtr<ID3D11ShaderResourceView> aShaderResourceView,
	const unsigned int& aWidth,
	const unsigned int& aHeight,
	unsigned char* aRgbaPixels,
	ComPtr<ID3D11Texture2D> aTexture
)
{
	myShaderResourceView = aShaderResourceView;
	myWidth = aWidth;
	myHeight = aHeight;
	myRgbaPixels = aRgbaPixels;
	myTexture = aTexture;
}

Texture::~Texture()
{
	delete[] myRgbaPixels;

	if (myTexture)
	{
		myTexture->Release();
	}

	if (myShaderResourceView)
	{
		myShaderResourceView->Release();
	}
}

void Texture::Bind(const PixelShaderTextureSlot& aSlot) const
{
	Bind(static_cast<unsigned int>(aSlot), true);
}

void Texture::Bind(const VertexShaderTextureSlot& aSlot) const
{
	Bind(static_cast<unsigned int>(aSlot), false);
}

void Texture::Bind(const unsigned int& aSlot, bool aShouldBindToPixelShader) const
{
	auto* context = GraphicsEngine::GetInstance()->DX().GetContext();

	if (myTexture != nullptr && myRgbaPixels != nullptr)
	{
		context->UpdateSubresource(myTexture.Get(), 0, nullptr, (void*)myRgbaPixels, myWidth * 4, myWidth * myHeight * 4);
	}

	if (aShouldBindToPixelShader)
	{
		context->PSSetShaderResources(aSlot, 1, myShaderResourceView.GetAddressOf());
	}
	else
	{
		context->VSSetShaderResources(aSlot, 1, myShaderResourceView.GetAddressOf());
	}
}

Vector2f Texture::GetTextureSize() const
{
	ID3D11Resource* resource = nullptr;
	myShaderResourceView->GetResource(&resource);
	if (!resource)
	{
		return Vector2f(0, 0);
	}
	ID3D11Texture2D* txt = reinterpret_cast<ID3D11Texture2D*>(resource);
	D3D11_TEXTURE2D_DESC desc;
	txt->GetDesc(&desc);

	Vector2f size(static_cast<float>(desc.Width), static_cast<float>(desc.Height));
	resource->Release();

	return size;
}

#ifdef _EDITOR
#include <engine/utility/Error.h>

void Texture::WriteToSRV(uint32_t* aRgbaPixels, unsigned int aOffsetWidth, unsigned int aOffsetHeight)
{
	auto* ge = GraphicsEngine::GetInstance();
	auto context = ge->DX().GetContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = context->Map(myTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		PrintE("[TextureFactory.cpp] Failed to map texture: ");
		return;
	}

	UINT* data = reinterpret_cast<UINT*>(mappedResource.pData);
	for (UINT i = aOffsetWidth + aOffsetHeight * myWidth; i < myWidth * myHeight; ++i)
	{
		data[i] = aRgbaPixels[i];
	}

	context->Unmap(myTexture.Get(), 0);
}
#endif
