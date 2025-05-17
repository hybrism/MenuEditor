#pragma once
#include "Renderer.h"
#include "../../math/Vector.h"
#include <directxtk/PrimitiveBatch.h>

namespace DirectX
{
	namespace DX11
	{
		struct VertexPositionColor;
	}
}

namespace DEBUG
{
	struct Line
	{
		DirectX::XMFLOAT3 from;
		DirectX::XMFLOAT3 to;
		DirectX::XMFLOAT4 color;
	};

	struct Quad
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 scale;
		DirectX::XMFLOAT4 color;
	};
}

class DebugRenderer : public Renderer
{
public:
	DebugRenderer();
	~DebugRenderer();

	void Init();

	void Render();
	void Clear();

	void DrawLine(const Vector3f& aFrom, const Vector3f& aTo, const Vector3f& aColor = { 1.f, 1.f, 1.f });
	void DrawLine(const Vector2f& aFrom, const Vector2f& aTo, const Vector3f& aColor = { 1.f, 1.f, 1.f });
	void DrawQuad(const Vector3f& aPosition, const Vector3f& aNormal, const Vector2f& aScale, const Vector3f& aColor = { 1.f, 1.f, 1.f });
private:
	std::vector<DEBUG::Line> my3DLines;
	std::vector<DEBUG::Quad> my3DQuads;
	std::vector<DEBUG::Line> my2DLines;

	Vector2f myRenderSize;
	std::unique_ptr<DirectX::DX11::PrimitiveBatch<DirectX::DX11::VertexPositionColor>> myPrimitiveBatch;
};