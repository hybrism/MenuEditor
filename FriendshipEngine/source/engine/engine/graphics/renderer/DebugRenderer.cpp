#include "pch.h"
#include "DebugRenderer.h"

#include <engine/utility/Error.h>
#include <engine/graphics/GraphicsEngine.h>

#include <directxtk/VertexTypes.h>

DebugRenderer::DebugRenderer()
{}

DebugRenderer::~DebugRenderer()
{}

void DebugRenderer::Init()
{
	using namespace DirectX;

	auto ge = GraphicsEngine::GetInstance();
	auto device = ge->DX().GetDevice();
	auto context = ge->DX().GetContext();

	auto size = ge->DX().GetViewportDimensions();
	myRenderSize = { (float)size.x, (float)size.y };

	myPrimitiveBatch = std::make_unique<DX11::PrimitiveBatch<DX11::VertexPositionColor>>(context);
}

#include <engine/graphics/Camera.h>
#include <assets/ShaderDatabase.h>
#include <engine/shaders/VertexShader.h>
#include <engine/shaders/PixelShader.h>

void DebugRenderer::Render()
{
	using namespace DirectX;
	auto ge = GraphicsEngine::GetInstance();

	RenderState renderState;
	renderState.blendState = BlendState::Disabled;
	renderState.depthStencilState = DepthStencilState::ReadWrite;
	renderState.rasterizerState = RasterizerState::WireframeNoCulling;

	ge->SetRenderState(renderState);

	ShaderDatabase::GetPixelShader(PsType::DebugLine)->PrepareRender();

	// 3D
	ShaderDatabase::GetVertexShader(VsType::DebugLine3D)->PrepareRender();
	myPrimitiveBatch->Begin();
	{

		//TODO Fix Depth and Rendertarget for 3dLines!
		for (const DEBUG::Line& line : my3DLines)
		{
			XMFLOAT3 from = { line.from.x, line.from.y, line.from.z };
			XMFLOAT3 to = { line.to.x, line.to.y, line.to.z };
			XMFLOAT4 color = { line.color.x, line.color.y, line.color.z, 1.f };

			myPrimitiveBatch->DrawLine(
				DX11::VertexPositionColor({ from, color }),
				DX11::VertexPositionColor({ to, color })
			);
		}

	}
	myPrimitiveBatch->End();

	// 3D Quads
	myPrimitiveBatch->Begin();
	{
		//TODO Fix Depth and Rendertarget for 3dLines!
		for (const DEBUG::Quad& quad : my3DQuads)
		{
			XMFLOAT3 position = quad.position;
			DirectX::XMVECTOR normal = { quad.normal.x, quad.normal.y, quad.normal.z };
			XMFLOAT2 scale = quad.scale;
			XMFLOAT4 color = quad.color;

			XMFLOAT3 q0 = XMFLOAT3(-scale.x / 2.0f, -scale.y / 2.0f, 0.0f);
			XMFLOAT3 q1 = XMFLOAT3(scale.x / 2.0f, -scale.y / 2.0f, 0.0f);
			XMFLOAT3 q2 = XMFLOAT3(scale.x / 2.0f, scale.y / 2.0f, 0.0f);
			XMFLOAT3 q3 = XMFLOAT3(-scale.x / 2.0f, scale.y / 2.0f, 0.0f);

			{
				XMVECTOR upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
				XMMATRIX viewMatrix = XMMatrixLookAtLH(XMVectorZero(), -normal, upVector);
				XMMATRIX rotationMatrix = XMMatrixInverse(nullptr, viewMatrix);


				XMVECTOR vertex = XMLoadFloat3(&q0);
				vertex = XMVector3TransformCoord(vertex, rotationMatrix);
				XMStoreFloat3(&q0, vertex);

				vertex = XMLoadFloat3(&q1);
				vertex = XMVector3TransformCoord(vertex, rotationMatrix);
				XMStoreFloat3(&q1, vertex);

				vertex = XMLoadFloat3(&q2);
				vertex = XMVector3TransformCoord(vertex, rotationMatrix);
				XMStoreFloat3(&q2, vertex);

				vertex = XMLoadFloat3(&q3);
				vertex = XMVector3TransformCoord(vertex, rotationMatrix);
				XMStoreFloat3(&q3, vertex);
			}

			myPrimitiveBatch->DrawQuad(
				DX11::VertexPositionColor({ q0.x + position.x, q0.y + position.y, q0.z + position.z }, color),
				DX11::VertexPositionColor({ q1.x + position.x, q1.y + position.y, q1.z + position.z }, color),
				DX11::VertexPositionColor({ q2.x + position.x, q2.y + position.y, q2.z + position.z }, color),
				DX11::VertexPositionColor({ q3.x + position.x, q3.y + position.y, q3.z + position.z }, color)
			);
		}
	}
	myPrimitiveBatch->End();

	// 2D
	ShaderDatabase::GetVertexShader(VsType::DebugLine2D)->PrepareRender();
	myPrimitiveBatch->Begin();
	{

		renderState.depthStencilState = DepthStencilState::Disabled;

		ge->SetRenderState(renderState);

		for (const DEBUG::Line& line : my2DLines)
		{
			XMFLOAT3 from = { line.from.x, line.from.y, line.from.z };
			XMFLOAT3 to = { line.to.x, line.to.y, line.to.z };
			XMFLOAT4 color = { line.color.x, line.color.y, line.color.z, 1.f };

			myPrimitiveBatch->DrawLine(
				DX11::VertexPositionColor({ from, color }),
				DX11::VertexPositionColor({ to, color })
			);
		}

	}
	myPrimitiveBatch->End();

	//TODO: Perhaps move this to a PrepareRender? 
	my3DLines.clear();
	my3DQuads.clear();
	my2DLines.clear();
}

void DebugRenderer::Clear()
{
	my3DLines.clear();
	my3DQuads.clear();
	my2DLines.clear();
}

void DebugRenderer::DrawLine(const Vector3f& aFrom, const Vector3f& aTo, const Vector3f& aColor)
{
	DirectX::XMFLOAT3 from = { aFrom.x, aFrom.y, aFrom.z };
	DirectX::XMFLOAT3 to = { aTo.x, aTo.y, aTo.z };
	DirectX::XMFLOAT4 color = { aColor.x, aColor.y, aColor.z, 1.f };

	my3DLines.push_back(DEBUG::Line({ from, to, color }));
}

void DebugRenderer::DrawLine(const Vector2f& aFrom, const Vector2f& aTo, const Vector3f& aColor)
{
	DirectX::XMFLOAT3 from = { aFrom.x, aFrom.y, 0.f };
	DirectX::XMFLOAT3 to = { aTo.x, aTo.y, 0.f };
	DirectX::XMFLOAT4 color = { aColor.x, aColor.y, aColor.z, 1.f };

	my2DLines.push_back(DEBUG::Line({ from, to, color }));
}

void DebugRenderer::DrawQuad(const Vector3f& aPosition, const Vector3f& aNormal, const Vector2f& aScale, const Vector3f& aColor)
{
	DirectX::XMFLOAT3 position = { aPosition.x, aPosition.y, aPosition.z };
	DirectX::XMFLOAT3 normal = { aNormal.x, aNormal.y, aNormal.z };
	DirectX::XMFLOAT2 scale = { aScale.x, aScale.y };
	DirectX::XMFLOAT4 color = { aColor.x, aColor.y, aColor.z, 1.f };

	my3DQuads.push_back(DEBUG::Quad({ position, normal, scale, color }));
}
