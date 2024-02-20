#pragma once
#include "SharedMesh.h"

class StaticModelShader;

class Mesh : public SharedMesh
{
public:
	Mesh();
	~Mesh() override;
	bool Initialize(
		Vertex aVertices[],
		const size_t& aVertexCount,
		unsigned int[],
		const size_t& aIndexCount,
		const bool& aUseDynamicVertexBuffer = false
	);

	void UpdateVertices(Vertex aVertices[], const unsigned int& aVertexCount);

	void Render(const DirectX::XMMATRIX& aTransform, const VertexShader* aVS, const PixelShader* aPS, const RenderMode& aRenderMode) const override;
private:
};