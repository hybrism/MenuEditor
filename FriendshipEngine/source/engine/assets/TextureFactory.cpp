#include "pch.h"

#define STB_IMAGE_IMPLEMENTATION
#include "TextureFactory.h"
#include <stb/stb_image.h>
#include "DDSTextureLoader11.h"
#include <engine/graphics/Texture.h>

TextureFactory::TextureFactory() = default;
TextureFactory::~TextureFactory() = default;

Texture* TextureFactory::CreateTexture(const std::string& aTexturePath, const bool& aUseRelative)
{
	std::string path = aTexturePath;
	if (aUseRelative)
	{
		path = RELATIVE_ASSET_PATH + path;
	}

	if (StringHelper::GetFileExtension(path) == "png")
	{
		Texture* texture = nullptr;
		if (CreatePNGTexture(path, texture))
		{
			return texture;
		}
	}
	else
	{
		ComPtr<ID3D11ShaderResourceView> shaderResourceView;
		if (CreateDDSTexture(StringHelper::s2ws(path), shaderResourceView))
		{
			return new Texture(shaderResourceView);
		}
	}

	PrintPathError(path);

	//// TODO: Proper error handling
	//assert("Failed to init texture" && false);

	return nullptr;
}

bool TextureFactory::CreatePNGTexture(const std::string& aPath, Texture*& outTexture)
{
	bool useSrgb = true; // temp?
	int width, height, channels;

	unsigned char* rgbaPixels = stbi_load(aPath.c_str(), &width, &height, &channels, 0);
	if (rgbaPixels == nullptr)
		return false;

	if (channels < 3 || channels > 4)
		return false;

	if (channels == 3)
	{
		std::vector<unsigned char> imageData(width * height * 4);
		for (int i = 0; i < width * height; i++)
		{
			imageData[4 * i + 0] = rgbaPixels[3 * i + 0];
			imageData[4 * i + 1] = rgbaPixels[3 * i + 1];
			imageData[4 * i + 2] = rgbaPixels[3 * i + 2];
			imageData[4 * i + 3] = 255;
		}
		rgbaPixels = imageData.data();
	}

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	if (useSrgb)
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	else
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	auto device = GraphicsEngine::GetInstance()->GetDevice();

	ID3D11Texture2D* myTexture;
	HRESULT hra = device->CreateTexture2D(&desc, nullptr, &myTexture);
	if (FAILED(hra))
	{
		return false;
	}

	ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
	HRESULT hr = device->CreateShaderResourceView(myTexture, NULL, &myShaderResourceView);
	if (FAILED(hr))
	{
		return false;
	}

	GraphicsEngine::GetInstance()->GetContext()->GenerateMips(myShaderResourceView.Get());
	myTexture->Release();

	outTexture = new Texture(myShaderResourceView, width, height, rgbaPixels, myTexture);

	return true;
}

bool TextureFactory::CreateDDSTexture(const std::wstring& aPath, ComPtr<ID3D11ShaderResourceView>& outShaderResourceView)
{
	auto device = GraphicsEngine::GetInstance()->GetDevice();
	HRESULT result = DirectX::CreateDDSTextureFromFile(
		GraphicsEngine::GetInstance()->GetDevice().Get(),
		aPath.c_str(),
		nullptr,
		&outShaderResourceView
	);
	return !FAILED(result);
}
