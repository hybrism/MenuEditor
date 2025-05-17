#pragma once
#include <wrl/client.h>
#include "../math/Vector.h"

enum class PixelShaderTextureSlot;
enum class VertexShaderTextureSlot;

using Microsoft::WRL::ComPtr;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

#ifdef _EDITOR
	struct SRVWriteData
	{
		unsigned int index;
		uint32_t pixel;
	};
#endif

// TODO: Add GetFileDimensions()
class Texture
{
	friend class TextureFactory;
public:
	// DDS
	Texture(ComPtr<ID3D11ShaderResourceView> aShaderResourceView);
	Texture(
		ComPtr<ID3D11ShaderResourceView> aShaderResourceView,
		const unsigned int& aWidth,
		const unsigned int& aHeight
	);
	Texture(
		ComPtr<ID3D11ShaderResourceView> aShaderResourceView,
		const unsigned int& aWidth,
		const unsigned int& aHeight,
		ComPtr<ID3D11Texture2D> aTexture
	);

	// PNG
	Texture(
		ComPtr<ID3D11ShaderResourceView> aShaderResourceView,
		const unsigned int& aWidth,
		const unsigned int& aHeight,
		unsigned char* aRgbaPixels,
		ComPtr<ID3D11Texture2D> aTexture
	);
	~Texture();

	void Bind(const unsigned int& aSlot, bool aShouldBindToPixelShader = true) const;
	void Bind(const PixelShaderTextureSlot& aSlot) const;
	void Bind(const VertexShaderTextureSlot& aSlot) const;
	Vector2f GetTextureSize() const;
	ID3D11ShaderResourceView* GetShaderResourceView() const { return myShaderResourceView.Get(); };
	
	unsigned int GetWidth() const { return myWidth; }
	unsigned int GetHeight() const { return myHeight; }

	// TEMP: SHOULD BE REMOVED AFTER MODIFYING SRV DIRECTLY
#ifdef _EDITOR
	void SetShaderResourceView(ComPtr<ID3D11ShaderResourceView> aShaderResourceView) { myShaderResourceView = aShaderResourceView; }
	void WriteToSRV(uint32_t* aRgbaPixels, unsigned int aOffsetWidth = 0, unsigned int aOffsetHeight = 0);
	ComPtr<ID3D11Texture2D> GetTexture() const { return myTexture; }
#endif
private:
	unsigned int myWidth, myHeight;
	unsigned char* myRgbaPixels = nullptr;
	ComPtr<ID3D11Texture2D> myTexture;
	ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
};