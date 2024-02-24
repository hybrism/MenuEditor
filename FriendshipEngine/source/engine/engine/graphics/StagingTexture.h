#pragma once
struct ID3D11Texture2D;
struct D3D11_MAPPED_SUBRESOURCE;

struct StagingTexture
{
public:
#ifdef _DEBUG
	void CopyToStaging() const;
	bool Map(D3D11_MAPPED_SUBRESOURCE& outData) const;
	void Unmap() const;
	ID3D11Texture2D* GetStagingTexture() const { return myStagingTexture; }
#endif

	ID3D11Texture2D* myTexture = nullptr;
#ifdef _DEBUG
	ID3D11Texture2D* myStagingTexture = nullptr;
#endif
protected:
	void CreateStagingTexture();
};