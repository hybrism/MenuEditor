#pragma once
#include "Renderer.h"

#include <unordered_map>
#include <array>

#include "../model/MeshDrawer.h"
#include "../model/MeshDrawerStructs.h"
#include "../RenderDefines.h"

class Mesh;
class SkeletalMesh;

#define SHOULD_SORT_ALPHA_LAYERS 0

class ForwardRenderer : public Renderer
{
public:
	ForwardRenderer(MeshDrawer& aMeshDrawer);
	~ForwardRenderer() override;

	void Render(Mesh* aMesh, const MeshInstanceRenderData& aInstanceData, BlendState aBlendState = BlendState::Disabled);
	void Render(SkeletalMesh* aMesh, const MeshInstanceRenderData& aInstanceData, BlendState aBlendState = BlendState::Disabled);

	// TODO: add support to disable shadows on one instance
	void DoShadowRenderPass();
	void DoRenderPass();
private:
	void Clear();
	// TODO: add support to render different shaders for each instance
	void RenderMeshes(bool aUsePixelShader);


	// TODO: när vi tar bort unity eller ändrar hur meshIds fungerar
	// så borde vi byta till att använda en array som är lika stor som antalet meshes i systemet
	std::array<std::unordered_map<Mesh*, std::vector<MeshInstanceRenderData>>, (size_t)BlendState::Count> myStaticMeshes;
	std::array<std::unordered_map<SkeletalMesh*, std::vector<MeshInstanceRenderData>>, (size_t)BlendState::Count> mySkeletalMeshes;

#if SHOULD_SORT_ALPHA_LAYERS
	std::vector<MeshInstanceRenderData*> myAlphaBlendingInstanceDataContainer;
#endif
	MeshDrawer& myMeshDrawer;
};