#include "pch.h"
#include "Texture.h"
#include "GraphicsEngine.h"
#include "RenderDefines.h"

Texture::Texture(ComPtr<ID3D11ShaderResourceView> aShaderResourceView)
{
	myShaderResourceView = aShaderResourceView;
	myWidth = 0;
	myHeight = 0;
	myRgbaPixels = nullptr;
	myTexture = nullptr;
}

Texture::Texture(
	ComPtr<ID3D11ShaderResourceView> aShaderResourceView,
	const int& aWidth,
	const int& aHeight,
	unsigned char* aRgbaPixels,
	ID3D11Texture2D* aTexture
)
{
	myShaderResourceView = aShaderResourceView;
	myWidth = aWidth;
	myHeight = aHeight;
	myRgbaPixels = aRgbaPixels;
	myTexture = aTexture;
}

Texture::~Texture() = default;

void Texture::Bind(const ShaderTextureSlot& aSlot) const
{
	Bind(static_cast<int>(aSlot));
}

void Texture::Bind(const int& aSlot) const
{
	auto* context = GraphicsEngine::GetInstance()->GetContext();

	if (myTexture != nullptr)
	{
		context->UpdateSubresource(myTexture, 0, nullptr, (void*)myRgbaPixels, myWidth * 4, myWidth * myHeight * 4);
	}

	context->PSSetShaderResources(aSlot, 1, myShaderResourceView.GetAddressOf());
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
