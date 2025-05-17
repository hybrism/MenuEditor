#pragma once
#include <memory>
#include "../DxBuffer.h"
#include "MeshDrawerStructs.h"

class SharedMesh;
class VertexShader;
class PixelShader;

class MeshDrawer
{
public:
	MeshDrawer();
	~MeshDrawer();
	void Init();

	void Draw(
		const SharedMesh& aSharedData,
		MeshInstanceRenderData& aInstance
	);
	void Draw(
		const SharedMesh& aSharedData,
		MeshInstanceRenderData& aInstance,
		const VertexShader* aVertexShader,
		const PixelShader* aPixelShader
	);
	void Draw(
		const SharedMesh& aSharedData,
		MeshInstanceRenderData* aInstances,
		const size_t& aInstanceDataCount
	);
	void Draw(
		const SharedMesh& aSharedData,
		MeshInstanceRenderData* aInstances,
		const size_t& aInstanceDataCount,
		const VertexShader* aVertexShader,
		const PixelShader* aPixelShader
	);
private:
	void InternalDraw(
		const SharedMesh& aSharedData,
		MeshInstanceRenderData* aInstances, // MUST BE ARRAY, example: StaticMeshInstanceData*
		const size_t& aInstanceDataCount,
		const VertexShader* aVertexShader,
		const PixelShader* aPixelShader
	);
	void UnMapAndRender(
		const SharedMesh& aSharedData,
		size_t& aInstanceCount,
		void*& aMappableData,
		D3D11_PRIMITIVE_TOPOLOGY aTopology
	);
	void Map(void*& aMappableData);

	const DxBuffer* myCurrentInstanceBuffer;
	DxBuffer myInstanceBuffer[(int)InstanceDataType::Count];
};
