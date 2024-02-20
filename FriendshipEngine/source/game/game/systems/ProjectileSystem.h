#pragma once
#include <ecs\World.h>
#include "../component/ProjectileComponent.h"
#include "../component/TransformComponent.h"
#include "../component/MeshComponent.h"
#include "../component/MetaDataComponent.h"

class ProjectileSystem : public System
{
public:
	ProjectileSystem(World* aWorld);

	void Init() override;
	void Update(const float& dt) override;
};