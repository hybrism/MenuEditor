#pragma once
#include <wrl/client.h>
#include <vector>
#include <string>
#include "../Vertex.h"
#include "../RenderDefines.h"
#include "../../../assets/AssetDefines.h"

using Microsoft::WRL::ComPtr;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

class VertexShader;
class PixelShader;

namespace DirectX
{
	struct XMMATRIX;
}

class SharedMesh
{
public:
	SharedMesh() = default;
	virtual ~SharedMesh() = default;

	virtual void Render(const DirectX::XMMATRIX& aTransform, const VertexShader* aVS, const PixelShader* aPS, const RenderMode& aRenderMode = RenderMode::TRIANGLELIST) const = 0;
	
	void SetTextures(const TextureCollection& aTextures);
	void SetVertexTextureId(const int aTextureId);
	int GetVertexTextureId() const { return myTextures.vertexPaintIndex; }
	const TextureCollection& GetTextures() const { return myTextures; }
		
	void SetMaterialName(std::string aMaterialName);
	const std::string GetMaterialName() const { return myMaterialName; }
	
	void SetFileName(const std::string& aFileName) { myFileName = aFileName; }
	const std::string GetFileName() const { return myFileName; }

	void SetVertexShader(const VertexShader* aVertexShader) { myVertexShader = aVertexShader; }
	void SetPixelShader(const PixelShader* aPixelShader) { myPixelShader = aPixelShader; }

	const VertexShader* GetVertexShader() const { return myVertexShader; }
	const PixelShader* GetPixelShader() const { return myPixelShader; }

	ID3D11Buffer* GetVertexBuffer() const { return myVertexBuffer.Get(); }
	ID3D11Buffer* GetIndexBuffer() const { return myIndexBuffer.Get(); }

	unsigned int GetIndexCount() const { return myIndexCount; }
	unsigned int GetVertexSize() const { return myVertexSize; }

	void BindTextures() const;

#ifdef _EDITOR
	Vertex* GetVertices() { return myVertices; }
	unsigned int GetVertexCount() { return myVertexCount; }
#endif
protected:
#ifdef _EDITOR
	Vertex* myVertices = nullptr;
	unsigned int myVertexCount = 0;
#endif
	unsigned int* myIndices = nullptr;
	unsigned int myIndexCount = 0;

	// sizeof(Vertex) etc
	unsigned int myVertexSize = 0;

	const VertexShader* myVertexShader = nullptr;
	const PixelShader* myPixelShader = nullptr;

	std::string myFileName = "";
	std::string myMaterialName = ""; // we will only use one material per mesh
	TextureCollection myTextures{};

	ComPtr<ID3D11Buffer> myVertexBuffer = NULL;
	ComPtr<ID3D11Buffer> myIndexBuffer = NULL;
	ComPtr<ID3D11InputLayout> myInputLayout = NULL;
};