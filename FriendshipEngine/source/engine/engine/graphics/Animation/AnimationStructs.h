#pragma once
#include <cstdint>
#include "Pose.h"

#define ANIMATION_DATA_PARAMETER_COUNT 4

union AnimationDataParameter
{
	float f;
	bool b;

	AnimationDataParameter() : f(0) { }
	AnimationDataParameter(float aValue) : f(aValue) { }
	AnimationDataParameter(int aValue) : f(static_cast<float>(aValue)) { }
	AnimationDataParameter(bool aValue) : b(aValue) { }
};

typedef AnimationDataParameter AnimationDataParameterContainer[ANIMATION_DATA_PARAMETER_COUNT];

struct AnimationData
{
	Pose localSpacePose = {};

	float time[2] = { 0.0f, 0.0f };
	float speed = 1.0f;

	int transitionIndex = -1;
	int previousStateIndex = -1;
	int currentStateIndex = -1;
	int nextStateIndex = -1;
	float exitTimer = 0.0f;
	float transitionTimer = 0.0f;
	float blendFactor = 0.0f;
	bool isPlaying = true;
	bool isDone = false;

	bool IsTransitioning() const
	{
		return transitionIndex != -1;
	}
};

