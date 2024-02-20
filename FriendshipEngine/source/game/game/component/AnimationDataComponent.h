#pragma once
#include <ecs/component/Component.h>
#include <string>
#include <engine/graphics/animation/AnimationStructs.h>

// blackboard for conditions?

struct AnimationDataComponent : public Component<AnimationDataComponent>, public AnimationData
{
	AnimationDataParameterContainer parameters;
};
