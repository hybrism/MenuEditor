#pragma once
#include "../../math/Transform.h"
#include <vector>
#include <string>
#include <unordered_map>

struct Frame
{
	std::unordered_map<int, Transform> localTransforms;
};

struct Animation
{
	std::vector<Frame> frames;
	std::string name;
	unsigned int length;
	float frameRate;
	float duration;
};