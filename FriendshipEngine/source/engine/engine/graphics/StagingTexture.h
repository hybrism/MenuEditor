#pragma once
struct ID3D11Texture2D;
struct D3D11_MAPPED_SUBRESOURCE;

using Microsoft::WRL::ComPtr;

struct StagingTexture
{
public:
	void CopyToStaging() const;
#ifdef _EDITOR
	bool Map(D3D11_MAPPED_SUBRESOURCE& outData) const;
	void Unmap() const;
	ID3D11Texture2D* GetStagingTexture() const { return myStagingTexture; }
#endif

	ID3D11Texture2D* myTexture = nullptr;
	ID3D11Texture2D* myIntermediateTexture = nullptr;
	ComPtr<ID3D11ShaderResourceView> SRV;
	ComPtr<ID3D11ShaderResourceView> myIntermediateSRV;
#ifdef _EDITOR
	ID3D11Texture2D* myStagingTexture = nullptr;
#endif
protected:
	void CreateStagingTexture(D3D11_SHADER_RESOURCE_VIEW_DESC* aSRVDesc);
};