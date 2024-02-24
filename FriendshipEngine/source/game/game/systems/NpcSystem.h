#pragma once
#include <ecs\World.h>
#include "../component/TransformComponent.h"
#include "../component/MeshComponent.h"
#include "../component/MetaDataComponent.h"
#include <engine\math\Vector.h>

struct NpcComponent;
struct TransformComponent;
enum class npcState;

class NpcSystem : public System
{
public:
	NpcSystem(World* aWorld);
	
	void Init() override;
	void Update(const float& dt) override;
	void FollowPath(const float& dt,NpcComponent& aNpc,TransformComponent& aTransform);
	void ActivateGroup(int aGroupID);

private:

};

