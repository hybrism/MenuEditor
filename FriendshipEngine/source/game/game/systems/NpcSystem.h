#pragma once
#include <ecs\World.h>
#include "../component/TransformComponent.h"
#include "../component/MeshComponent.h"
#include "../component/MetaDataComponent.h"
#include <engine\math\Vector.h>

struct NpcComponent;
struct TransformComponent;
enum class npcState;

class NpcSystem : public System<NpcSystem>
{
public:
	NpcSystem(World* aWorld);
	
	void Init() override;
	void Update(SceneUpdateContext& dt) override;
	void FollowPath(const float& dt);
	void ActivateGroup(int aGroupID);

private:
	DirectX::XMMATRIX myMatrix;

};

