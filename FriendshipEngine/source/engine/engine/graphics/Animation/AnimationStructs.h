#pragma once
#include <cstdint>
#include "Pose.h"

#define ANIMATION_DATA_PARAMETER_COUNT 4

union AnimationDataParameter
{
	float f;
	int i;
};

typedef AnimationDataParameter AnimationDataParameterContainer[ANIMATION_DATA_PARAMETER_COUNT];

struct AnimationData
{
	Pose localSpacePose;

	float time = 0.0f;
	float speed = 1.0f;

	int transitionIndex = -1;
	int previousStateIndex = -1;
	int currentStateIndex = -1;
	int nextStateIndex = -1;
	float exitTimer = 0.0f;
	float transitionTimer = 0.0f;
};

