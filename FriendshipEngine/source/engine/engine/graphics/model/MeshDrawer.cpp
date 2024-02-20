#include "pch.h"
#include "MeshDrawer.h"

#include <d3d11.h>
#include <string>
#include <cassert>

#include "../../shaders/VertexShader.h"
#include "../../shaders/PixelShader.h"
#include "../../Paths.h"
#include "../../utility/Error.h"
#include "../GraphicsEngine.h"
#include "../../../assets/ShaderDatabase.h"

#include "MeshDrawerStructs.h"
#include "SharedMesh.h"

// maybe this should be lower for skeletal meshes?
constexpr size_t BATCH_SIZE = 1024;

MeshDrawer::MeshDrawer() = default;
MeshDrawer::~MeshDrawer() = default;

void MeshDrawer::UnMapAndRender(
	const SharedMesh& aSharedData,
	size_t& aInstanceCount,
	void*& aMappableData,
	D3D11_PRIMITIVE_TOPOLOGY aTopology
)
{
	ID3D11DeviceContext* context = GraphicsEngine::GetInstance()->GetContext();

	context->IASetPrimitiveTopology(aTopology);

	myCurrentInstanceBuffer->UnMap((void*&)aMappableData);

	context->DrawIndexedInstanced((UINT)aSharedData.GetIndexCount(), (UINT)aInstanceCount, 0, 0, 0);
#ifdef _DEBUG
	GraphicsEngine::GetInstance()->IncrementDrawCalls();
#endif
	aInstanceCount = 0;
}

void MeshDrawer::Map(void*& aMappableData)
{
	myCurrentInstanceBuffer->Map((void*&)aMappableData);
}

void MeshDrawer::InternalDraw(
	const SharedMesh& aSharedData,
	MeshInstanceRenderData* aInstances,
	const size_t& aInstanceCount,
	const VertexShader* aVertexShader,
	const PixelShader* aPixelShader
)
{
	myCurrentInstanceBuffer = &myInstanceBuffer;
	if (aVertexShader->GetInstanceBuffer().HasBuffer())
	{
		myCurrentInstanceBuffer = &aVertexShader->GetInstanceBuffer();
	}

	void* currentMappedInstanceDataCluster = nullptr;
	auto* ge = GraphicsEngine::GetInstance();
	auto* context = ge->GetContext();

	{
		aSharedData.BindTextures();

		unsigned int strides[2] { aSharedData.GetVertexSize(), myCurrentInstanceBuffer->myStride };
		unsigned int offsets[2] { 0, myCurrentInstanceBuffer->myOffset };
		ID3D11Buffer* bufferContainer[2] { aSharedData.GetVertexBuffer(), myCurrentInstanceBuffer->myBuffer.Get() };

		context->IASetVertexBuffers(0, 2, bufferContainer, strides, offsets);
		context->IASetIndexBuffer(aSharedData.GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		if (aVertexShader) { aVertexShader->PrepareRender(); }
		else { ge->GetContext()->VSSetShader(nullptr, NULL, 0); }
		if (aPixelShader) { aPixelShader->PrepareRender(); }
		else { ge->GetContext()->PSSetShader(nullptr, NULL, 0); }
	}

	Map(currentMappedInstanceDataCluster);

	size_t instanceCount = 0;
	size_t counter = 0;

	do
	{
		memcpy((char*)currentMappedInstanceDataCluster + myCurrentInstanceBuffer->myStride * instanceCount, &aInstances[counter].data, myCurrentInstanceBuffer->myStride);

		++instanceCount;
		++counter;

		if (instanceCount >= BATCH_SIZE)
		{
			UnMapAndRender(aSharedData, instanceCount, currentMappedInstanceDataCluster, (D3D11_PRIMITIVE_TOPOLOGY)aInstances[counter].renderMode);
			Map(currentMappedInstanceDataCluster);

			instanceCount = 0;
		}
	} while (counter < aInstanceCount);

	if (instanceCount > 0)
	{
		UnMapAndRender(aSharedData, instanceCount, currentMappedInstanceDataCluster, (D3D11_PRIMITIVE_TOPOLOGY)aInstances[counter - 1].renderMode);
	}
}

void MeshDrawer::Init()
{
	D3D11_BUFFER_DESC objectBufferDesc;
	objectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objectBufferDesc.ByteWidth = sizeof(StaticMeshInstanceData) * BATCH_SIZE;
	objectBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	objectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	objectBufferDesc.MiscFlags = 0;
	objectBufferDesc.StructureByteStride = 0;

	myInstanceBuffer.Init(objectBufferDesc, sizeof(StaticMeshInstanceData));
	myCurrentInstanceBuffer = &myInstanceBuffer;
}

void MeshDrawer::Draw(
	const SharedMesh& aSharedData,
	MeshInstanceRenderData& aInstance
)
{
	InternalDraw(aSharedData, &aInstance, 1, aSharedData.GetVertexShader(), aSharedData.GetPixelShader());
}

void MeshDrawer::Draw(
	const SharedMesh& aSharedData,
	MeshInstanceRenderData& aInstance,
	const VertexShader* aVertexShader,
	const PixelShader* aPixelShader
)
{
	InternalDraw(aSharedData, &aInstance, 1, aVertexShader, aPixelShader);
}

void MeshDrawer::Draw(
	const SharedMesh& aSharedData,
	MeshInstanceRenderData* aInstances,
	const size_t& aInstanceDataCount
)
{
	InternalDraw(aSharedData, aInstances, aInstanceDataCount, aSharedData.GetVertexShader(), aSharedData.GetPixelShader());
}

void MeshDrawer::Draw(
	const SharedMesh& aSharedData,
	MeshInstanceRenderData* aInstances,
	const size_t& aInstanceDataCount,
	const VertexShader* aVertexShader,
	const PixelShader* aPixelShader
)
{
	InternalDraw(aSharedData, aInstances, aInstanceDataCount, aVertexShader, aPixelShader);
}
