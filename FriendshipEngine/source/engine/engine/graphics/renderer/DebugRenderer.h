#pragma once
#include "Renderer.h"
#include "../../math/Vector.h"
#include <directxtk/PrimitiveBatch.h>

namespace DirectX
{
	namespace DX11
	{
		struct VertexPositionColor;
		class BasicEffect;
		class CommonStates;
	}
}

class DebugRenderer : public Renderer
{
	struct Line
	{
		DirectX::XMFLOAT3 from;
		DirectX::XMFLOAT3 to;
		DirectX::XMFLOAT4 color;
	};

public:
	DebugRenderer();
	~DebugRenderer();

	void Init();

	void Render();

	void DrawLine(const Vector3f& aFrom, const Vector3f& aTo, const Vector3f& aColor = { 1.f, 1.f, 1.f });
	void DrawLine(const Vector2f& aFrom, const Vector2f& aTo, const Vector3f& aColor = { 1.f, 1.f, 1.f });

private:
	std::vector<Line> my3DLines;
	std::vector<Line> my2DLines;

	Vector2f myRenderSize;
	const Vector3f WHITE = { 1.f, 1.f, 1.f };

	std::unique_ptr<DirectX::DX11::PrimitiveBatch<DirectX::DX11::VertexPositionColor>> myPrimitiveBatch;
	std::unique_ptr<DirectX::DX11::BasicEffect> myBasicEffect;
	std::unique_ptr<DirectX::DX11::CommonStates> myStates;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> myInputLayout;

};