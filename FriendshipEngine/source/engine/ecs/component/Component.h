#pragma once
#include <bitset>

#ifdef _DEBUG
#include "../entity/Entity.h"
#endif

const unsigned long MAX_COMPONENTS = 64;
typedef std::bitset<MAX_COMPONENTS> ComponentSignature;
typedef unsigned long int cid_t;

struct EntitySignature
{
	int indices[MAX_COMPONENTS] = {};
	ComponentSignature signature = {};
};

template<typename T>
class ComponentContainer;

// CRTP (Curiously Recurring Template Pattern) exploit >:3c
template <typename Derived>
struct Component
{
private:
	friend class ComponentManager;
	friend class ComponentContainer<Derived>;
	static cid_t componentId;
public:
	Component() = default;
	Component(const Component& aComponent) = delete;
	Component& operator=(const Component& aComponent) = delete;
#ifdef _DEBUG
	Entity myOwner = 0;
	EntityData* myOwnerData = nullptr;
#endif
};

// definition of the static variable
template <typename Derived>
cid_t Component<Derived>::componentId = 0;
