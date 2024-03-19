#pragma once
#include "Renderer.h"

#include <unordered_map>
#include <vector>

#include "../model/MeshDrawer.h"
#include "../model/MeshDrawerStructs.h"

class Mesh;
class SkeletalMesh;

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
	void DoGBufferPass();
	void DoFinalPass();
private:
	void Clear();
	void PrepareGBuffer();

	// TODO: add support to render different shaders for each instance
	void RenderMeshes(bool aUsePixelShader);
	void RenderDeferred();


	// TODO: när vi tar bort unity eller ändrar hur meshIds fungerar
	// så borde vi byta till att använda en array som är lika stor som antalet meshes i systemet

	// TODO: optimize this since push_back is EXTREMLY slow D:
	std::unordered_map<Mesh*, std::vector<MeshInstanceRenderData>> myStaticMeshes;
	std::unordered_map<SkeletalMesh*, std::vector<MeshInstanceRenderData>> mySkeletalMeshes;
	std::unordered_map<SkeletalMesh*, std::vector<MeshInstanceRenderData>> myDisregardDepthMeshes;
	MeshDrawer& myMeshDrawer;
};