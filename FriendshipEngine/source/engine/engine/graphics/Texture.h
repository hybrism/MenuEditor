#pragma once
#include <wrl/client.h>
#include "../math/Vector.h"

enum class PixelShaderTextureSlot;
enum class VertexShaderTextureSlot;

using Microsoft::WRL::ComPtr;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

// TODO: Add GetFileDimensions()
class Texture
{
public:
	// DDS
	Texture(ComPtr<ID3D11ShaderResourceView> aShaderResourceView);
	Texture(
		ComPtr<ID3D11ShaderResourceView> aShaderResourceView,
		const int& aWidth,
		const int& aHeight
	);

	// PNG
	Texture(
		ComPtr<ID3D11ShaderResourceView> aShaderResourceView,
		const int& aWidth,
		const int& aHeight,
		unsigned char* aRgbaPixels,
		ID3D11Texture2D* aTexture
	);
	~Texture();
	void Bind(const unsigned int& aSlot, bool aShouldBindToPixelShader = true) const;
	void Bind(const PixelShaderTextureSlot& aSlot) const;
	void Bind(const VertexShaderTextureSlot& aSlot) const;
	Vector2f GetTextureSize() const;
	ID3D11ShaderResourceView* GetShaderResourceView() const { return myShaderResourceView.Get(); };

	// TEMP: SHOULD BE REMOVED AFTER MODIFYING SRV DIRECTLY
#ifdef _EDITOR
	void SetShaderResourceView(ComPtr<ID3D11ShaderResourceView> aShaderResourceView) { myShaderResourceView = aShaderResourceView; }
#endif
private:
	int myWidth, myHeight;
	unsigned char* myRgbaPixels = nullptr;
	ID3D11Texture2D* myTexture = nullptr;
	ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
};