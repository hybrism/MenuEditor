#include "pch.h"
#include "Mesh.h"
#include "../GraphicsEngine.h"
#include "../../utility/Error.h"
#include "../GraphicsData.h"

#include "../../shaders/PixelShader.h"
#include "../../shaders/VertexShader.h"

Mesh::Mesh() = default;
Mesh::~Mesh()
{

}

bool Mesh::Initialize(
	Vertex aVerts[],
	const size_t& aVertexCount,
	unsigned int aIndices[],
	const size_t& aIndexCount,
	const bool& aUseDynamicVertexBuffer
)
{
	myVertexSize = sizeof(Vertex);

	HRESULT result;
	auto device = GraphicsEngine::GetInstance()->GetDevice();
	{
		// Create vertex buffer
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.ByteWidth = static_cast<UINT>(myVertexSize * aVertexCount);
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		if (aUseDynamicVertexBuffer)
		{
			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}

		D3D11_SUBRESOURCE_DATA vertexData = { 0 };

		vertexData.pSysMem = aVerts;
		result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
		if (FAILED(result))
		{
			PrintE("Could not create Vertex Buffer [Mesh.cpp]");
			return false;
		}
	}

	{
		// Create index buffer
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(unsigned int) * aIndexCount);
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexData = { 0 };
		indexData.pSysMem = aIndices;
		// Create the index buffer.
		result = device->CreateBuffer(&indexBufferDesc, &indexData,
			&myIndexBuffer);
		if (FAILED(result))
		{
			PrintE("Could not create Vertex Buffer [Mesh.cpp]");
			return false;
		}
	}

	myIndexCount = static_cast<unsigned int>(aIndexCount);

	return true;
}

void Mesh::Render(const DirectX::XMMATRIX& aTransform, const VertexShader* aVS, const PixelShader* aPS, const RenderMode& aRenderMode) const
{
	auto* instance = GraphicsEngine::GetInstance();
	auto* context = instance->GetContext();
	auto objectBuffer = instance->GetObjectBuffer();

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	auto result = context->Map(objectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

	if (FAILED(result))
	{
		PrintE("Could not map ObjectBuffer [Mesh.cpp]");
		return;
	}

	ObjectBufferData* data = (ObjectBufferData*)mappedBuffer.pData;
	data->objectTransform = aTransform; // column 4 ger icke 0 resultat in i shadern????

	context->Unmap(objectBuffer.Get(), 0);
	context->VSSetConstantBuffers((UINT)BufferSlots::Object, 1, objectBuffer.GetAddressOf());
	context->PSSetConstantBuffers((UINT)BufferSlots::Object, 1, objectBuffer.GetAddressOf());

	BindTextures();

	if (aPS) { aPS->PrepareRender(); }
	if (aVS) { aVS->PrepareRender(); }


	unsigned int stride = myVertexSize;
	unsigned int offset = 0;

	context->IASetVertexBuffers((UINT)BufferSlots::Frame, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)aRenderMode);

	context->DrawIndexed(myIndexCount, 0, 0);
#ifdef _DEBUG
	instance->IncrementDrawCalls();
#endif
}

void Mesh::UpdateVertices(Vertex aVertices[], const unsigned int& aVertexCount)
{
	auto* instance = GraphicsEngine::GetInstance();
	auto* context = instance->GetContext();
	auto objectBuffer = instance->GetObjectBuffer();

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	auto result = context->Map(myVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	if (FAILED(result))
	{
		PrintE("Could not map VertexBuffer when changing its positions [Mesh.cpp]");
		return;
	}

	memcpy((Vertex*)mappedBuffer.pData, aVertices, myVertexSize * aVertexCount);

	context->Unmap(myVertexBuffer.Get(), 0);
}
