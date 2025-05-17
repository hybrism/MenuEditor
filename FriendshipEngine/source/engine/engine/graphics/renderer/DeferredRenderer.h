#pragma once
#include "Renderer.h"

#include <unordered_map>
#include <vector>

#include "../model/MeshDrawer.h"
#include "../model/MeshDrawerStructs.h"

class Mesh;
class SkeletalMesh;
class SharedMesh;

struct StaticData
{
	std::vector<MeshInstanceRenderData> data = {};
	size_t size = 0;
};

// TODO: Support render without depth
class DeferredRenderer : public Renderer
{
public:
	DeferredRenderer(MeshDrawer& aMeshDrawer);
	~DeferredRenderer() override;

	void Render(Mesh* aMesh, MeshInstanceRenderData aInstanceData);
	void Render(SkeletalMesh* aMesh, const MeshInstanceRenderData& aInstanceData, bool aShouldDisregardDepth = false);

	// TODO: add support to disable shadows on one instance
	void DoShadowRenderPass();
	void PrepareGBuffer();
	void DoGBufferPass();
	void DoFinalPass();
private:
	void Clear();

	void AddToRenderContainer(SharedMesh* aMesh, MeshInstanceRenderData aInstanceData, void* aContainer);

	// TODO: add support to render different shaders for each instance
	void RenderMeshes(bool aUsePixelShader);
	void RenderDeferred();


	// TODO: n�r vi tar bort unity eller �ndrar hur meshIds fungerar
	// s� borde vi byta till att anv�nda en array som �r lika stor som antalet meshes i systemet

	// TODO: optimize this since push_back is EXTREMLY slow D:
	std::unordered_map<SharedMesh*, StaticData> myStaticMeshes;
	std::unordered_map<SharedMesh*, StaticData> mySkeletalMeshes;
	std::unordered_map<SharedMesh*, StaticData> myDisregardDepthMeshes;
	MeshDrawer& myMeshDrawer;
};