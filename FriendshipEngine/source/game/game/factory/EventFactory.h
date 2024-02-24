#pragma once
#include <ecs/entity/Entity.h>
#include <engine/math/Vector.h>
#include "../component/EventComponent.h"

class World;

#ifdef CreateEvent
#undef CreateEvent
#endif

class EventFactory
{
public:
    // Static method to get the singleton instance
    static EventFactory& GetInstance();

    void Init(World* aWorld);
    //Hitboxes for enemy attacks
    Entity CreateEvent(eEvent aEventToTrigger, Entity aOwner);

private:
    // Private constructor to prevent instantiation
    EventFactory();
    ~EventFactory() = default;

    World* myWorld;

    // Static instance of the singleton
    static inline EventFactory* myInstance;
};