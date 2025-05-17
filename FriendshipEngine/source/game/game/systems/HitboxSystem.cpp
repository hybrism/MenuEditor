#include "pch.h"
#include "HitboxSystem.h"
#include <ecs/World.h>
#include <engine/utility/Error.h>



#include "../component/HitboxComponent.h"
#include "../component/CollisionDataComponent.h"



HitboxSystem::HitboxSystem(World* aWorld) : System(aWorld)
{

}

void HitboxSystem::Update(SceneUpdateContext& aContext)
{
	for (Entity entity : myEntities)
	{
		auto& hitBoxComponent = myWorld->GetComponent<HitboxComponent>(entity);
		auto& collisionDataComponent = myWorld->GetComponent<CollisionDataComponent>(entity);
		collisionDataComponent;
		hitBoxComponent.timer += aContext.dt;
		//std::string stringTimer = std::to_string(hitBoxComponent.timer);
		//When the timer goes over the duration or the hitbox is colliding destory it
		if (hitBoxComponent.shouldDestroyOnCollide)
		{
			if (collisionDataComponent.isColliding)
			{
				if (hitBoxComponent.didICollideLastFrame)
				{
					myWorld->DestroyEntity(entity);
					continue;
				}
				hitBoxComponent.didICollideLastFrame = true;
			}
		}
		if (hitBoxComponent.timer >= hitBoxComponent.lifetime)
		{
			myWorld->DestroyEntity(entity);
		}


	}
}
