#pragma once

#include "../entity/Entity.h"

#include <set>

class World;

// TODO: Maybe it would be a good idea to include a OnEntityAdded and OnEntityRemoved function?

class System
{
public:
	friend class SystemManager;
	System(World* aWorld) { myWorld = aWorld; }
	virtual ~System()
	{ 
		myEntities.clear();
		myWorld = nullptr;
	}

	void Reset()
	{
		myEntities.clear();
	}

	virtual void Init() { __noop; }
	virtual void Update(const float&) { __noop; }
	virtual void Render() { __noop; }

	//void AddEntity(Entity entity);
	//void RemoveEntity(Entity entity);

	const std::set<Entity>& GetEntities() const { return myEntities; }
protected:
	World* myWorld;
	std::set<Entity> myEntities;
};
