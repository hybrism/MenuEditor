#pragma once
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
struct ID3D11ShaderResourceView;

class Texture;

enum class TextureInitializationType
{
	DDS,
	PNG
};

class TextureFactory
{
public:
	TextureFactory();
	~TextureFactory();
	Texture* CreateTexture(const std::string& aTexturePath, const bool& aUseRelative = true);
	
	bool CreateDDSTexture(const std::wstring& aPath, ComPtr<ID3D11ShaderResourceView>& outShaderResourceView);

private:
	bool CreatePNGTexture(const std::string& aPath, Texture*& outTexture);
};