#pragma once
#include <engine/debug/DebugLine.h>
#include <engine/graphics/renderer/FrustumCulling.h>

class RenderingSystem : public System<RenderingSystem>
{
public:
	RenderingSystem(World* aWorld);
	~RenderingSystem() override;

	void Init() override;
	void Update(SceneUpdateContext&) override;
	void Render() override;

	bool CullObject(const Entity& aEntity);
private:
	FrustumCulling myFrustum;
};
