#pragma once
#include "../../math/Transform.h"
#include <vector>
#include <string>
#include <unordered_map>

typedef Pose Frame;

struct Animation
{
	std::vector<Frame> frames;
	std::string name;
	unsigned int length;
	float frameRate;
	float duration;
};