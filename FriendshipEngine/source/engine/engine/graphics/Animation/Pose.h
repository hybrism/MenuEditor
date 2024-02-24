#pragma once
#include "../../math/Transform.h"

constexpr size_t MAX_ANIMATION_BONES = 64;

struct Pose
{
	Transform jointTransforms[MAX_ANIMATION_BONES];
	size_t count = 0;
};
