#pragma once
#include <wrl/client.h>
#include "../math/Vector.h"

enum class ShaderTextureSlot;

using Microsoft::WRL::ComPtr;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

// TODO: Add GetFileDimensions()
class Texture
{
public:
	// DDS
	Texture(ComPtr<ID3D11ShaderResourceView> aShaderResourceView);

	// PNG
	Texture(
		ComPtr<ID3D11ShaderResourceView> aShaderResourceView,
		const int& aWidth,
		const int& aHeight,
		unsigned char* aRgbaPixels,
		ID3D11Texture2D* aTexture
	);
	~Texture();
	void Bind(const int& aSlot) const;
	void Bind(const ShaderTextureSlot& aSlot) const;
	Vector2f GetTextureSize() const;
	ID3D11ShaderResourceView* GetShaderResourceView() const { return myShaderResourceView.Get(); };

private:
	int myWidth, myHeight;
	unsigned char* myRgbaPixels = nullptr;
	ID3D11Texture2D* myTexture = nullptr;
	ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
};