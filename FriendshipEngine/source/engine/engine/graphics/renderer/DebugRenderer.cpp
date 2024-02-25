#include "pch.h"
#include "DebugRenderer.h"

#include <engine/utility/Error.h>
#include <engine/graphics/GraphicsEngine.h>

#include <directxtk/Effects.h>
#include <directxtk/VertexTypes.h>
#include <directxtk/CommonStates.h>


DebugRenderer::DebugRenderer()
{}

DebugRenderer::~DebugRenderer()
{}

void DebugRenderer::Init()
{
	using namespace DirectX;

	auto ge = GraphicsEngine::GetInstance();
	auto device = ge->GetDevice();
	auto context = ge->GetContext();

	auto size = ge->GetViewportDimensions();
	myRenderSize = { (float)size.x, (float)size.y };

	myPrimitiveBatch = std::make_unique<DX11::PrimitiveBatch<DX11::VertexPositionColor>>(context);
	myBasicEffect = std::make_unique<DX11::BasicEffect>(device.Get());
	myStates = std::make_unique<DX11::CommonStates>(device.Get());

	myBasicEffect->SetProjection(DirectX::XMMatrixOrthographicOffCenterRH(0,
		myRenderSize.x, myRenderSize.y, 0, 0, 1));

	myBasicEffect->SetVertexColorEnabled(true);

	void const* shaderByteCode;
	size_t byteCodeLength;

	myBasicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	HRESULT result = device->CreateInputLayout(
		DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		shaderByteCode,
		byteCodeLength,
		myInputLayout.ReleaseAndGetAddressOf()
	);

	if (FAILED(result))
		PrintW("[DebugRenderer.cpp] Failed to create DebugRenderer!");
}

void DebugRenderer::Render()
{
	using namespace DirectX;
	auto ge = GraphicsEngine::GetInstance();
	auto context = ge->GetContext();

	myPrimitiveBatch->Begin();

	//TODO Fix Depth and Rendertarget for 3dLines!
	for (const Line& line : my3DLines)
	{
		XMFLOAT3 from = { line.from.x, line.from.y, line.from.z };
		XMFLOAT3 to = { line.to.x, line.to.y, line.to.z };
		XMFLOAT4 color = { line.color.x, line.color.y, line.color.z, 1.f };

		myPrimitiveBatch->DrawLine(
			DX11::VertexPositionColor({ from, color }),
			DX11::VertexPositionColor({ to, color })
		);
	}

	context->OMSetBlendState(myStates->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(myStates->DepthNone(), 0);
	context->RSSetState(myStates->CullNone());

	myBasicEffect->Apply(context);

	context->IASetInputLayout(myInputLayout.Get());

	for (const Line& line : my2DLines)
	{
		XMFLOAT3 from = { line.from.x, line.from.y, line.from.z };
		XMFLOAT3 to = { line.to.x, line.to.y, line.to.z };
		XMFLOAT4 color = { line.color.x, line.color.y, line.color.z, 1.f };

		myPrimitiveBatch->DrawLine(
			DX11::VertexPositionColor({ from, color }),
			DX11::VertexPositionColor({ to, color })
		);
	}

	myPrimitiveBatch->End();

	//TODO: Perhaps move this to a PrepareRender? 
	my3DLines.clear();
	my2DLines.clear();
}

void DebugRenderer::DrawLine(const Vector3f& aFrom, const Vector3f& aTo, const Vector3f& aColor)
{
	DirectX::XMFLOAT3 from = { aFrom.x, aFrom.y, aFrom.z };
	DirectX::XMFLOAT3 to = { aTo.x, aTo.y, aTo.z };
	DirectX::XMFLOAT4 color = { aColor.x, aColor.y, aColor.z, 1.f };

	my3DLines.push_back(Line({ from, to, color }));
}

void DebugRenderer::DrawLine(const Vector2f& aFrom, const Vector2f& aTo, const Vector3f& aColor)
{
	DirectX::XMFLOAT3 from = { aFrom.x, aFrom.y, 0.f };
	DirectX::XMFLOAT3 to = { aTo.x, aTo.y, 0.f };
	DirectX::XMFLOAT4 color = { aColor.x, aColor.y, aColor.z, 1.f };

	my2DLines.push_back(Line({ from, to, color }));
}
