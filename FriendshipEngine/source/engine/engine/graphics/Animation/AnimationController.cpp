#include "pch.h"
#include "AnimationController.h"
#include "Animation.h"
#include <assets/AssetDatabase.h>

AnimationController::AnimationController(AssetDatabase* aAssetDatabase, int aId)
{
	myAssetDatabase = aAssetDatabase;
	myId = aId;
}

AnimationController::~AnimationController()
{
	myAnimationStates.clear();
	myParameterNameToIndexMap.clear();
	myAssetDatabase = nullptr;
}

void AnimationController::Update(const float& aDeltaTime, Animation& aAnimation, AnimationData& aData)
{
	if (!aData.isPlaying) { return; }

	aData.previousStateIndex = aData.currentStateIndex;
	if (aData.transitionIndex == -1) { return; }

	aData.exitTimer += aDeltaTime;
	auto* transition = GetCurrentState(aData).GetTransition(aData.transitionIndex);

	if (transition == nullptr) { return; }

	if (aData.exitTimer < transition->exitTime) { return; }

	aData.transitionTimer += aDeltaTime;
	if (transition->duration > 0)
	{
		float amount = aData.transitionTimer / transition->duration;
		if (!transition->fixedDuration)
		{ 
			amount *= aAnimation.duration;
		}

		aData.blendFactor = std::min(amount, 1.0f);
	}

	if (aData.transitionTimer < transition->duration) { return; }

	aData.currentStateIndex = aData.nextStateIndex;
	aData.transitionTimer = 0.0f;
	aData.exitTimer = 0.0f;
	aData.transitionIndex = -1;
	aData.blendFactor = 0.0f;
	aData.time[0] = aData.time[1];
	aData.time[1] = 0.0f;
}

size_t AnimationController::AddState(const int& aAnimationIndex)
{
	return AddState(AnimationState(aAnimationIndex));
}

size_t AnimationController::AddState(const AnimationState& aAnimationState)
{
	size_t index = myAnimationStates.size();

	myAnimationStates.push_back(aAnimationState);
#ifdef _EDITOR
	myAnimationStates[index].stateIndex = static_cast<int>(index);
#endif
	return index;
}

AnimationState& AnimationController::GetState(const size_t& aStateIndex)
{
	assert(aStateIndex < myAnimationStates.size() && "Index out of range");
	return myAnimationStates[aStateIndex];
}

AnimationState& AnimationController::GetCurrentState(AnimationData& aData)
{
	return myAnimationStates[aData.currentStateIndex];
}

void AnimationController::SetState(const unsigned int& aStateIndex, AnimationData& aData)
{
	assert(aStateIndex < myAnimationStates.size() && "Index out of range");
	aData.previousStateIndex = aData.currentStateIndex;
	aData.currentStateIndex = aStateIndex;
	aData.nextStateIndex = aData.currentStateIndex;
	aData.transitionIndex = -1;
	aData.blendFactor = 0.0f;
	aData.isDone = false;
	aData.time[0] = 0.0f;
	aData.time[1] = 0.0f;
}

int AnimationController::GetCurrentAnimationIndex(AnimationData& aData)
{
	return GetCurrentState(aData).GetAnimationIndex();
}

void AnimationController::RunTransitionCheck(AnimationData& aData, AnimationDataParameterContainer& aParameter)
{
	// if we are already transitioning, don't run another check
	if (aData.transitionIndex != -1) { return; }

	AnimationState& currentState = GetCurrentState(aData);
	size_t animationCount = currentState.GetTransitionCount();
	for (size_t i = 0; i < animationCount; i++)
	{
		bool shouldTransition = true;

		const AnimationTransition* transition = currentState.GetTransition(i);

		for (size_t j = 0; j < transition->conditions.size(); j++)
		{
			const AnimationCondition& condition = transition->conditions[j];
			if (!IsAnimationConditionMet(condition.parameterIndex, condition, aParameter))
			{
				shouldTransition = false;
				break;
			}
		}

		// prioritizes the first condition that is met
		if (shouldTransition)
		{
			aData.nextStateIndex = transition->toStateIndex;
			aData.transitionIndex = static_cast<int>(i);
			aData.transitionTimer = 0.0f;
			aData.exitTimer = 0.0f;
			aData.time[1] = 0.0f;
			break;
		}
	}
}

bool AnimationController::IsAnimationConditionMet(
	const size_t& aParameterIndex,
	const AnimationCondition& aCondition,
	AnimationDataParameterContainer& aParameters
) const
{
	const AnimationDataParameter& type = aParameters[aParameterIndex];

	switch (aCondition.type)
	{
	case AnimationConditionType::eIsEqual:
		return type.f == aCondition.target;
	case AnimationConditionType::eIsNotEqual:
		return type.f != aCondition.target;
	case AnimationConditionType::eIsGreaterThan:
		return type.f > aCondition.target;
	case AnimationConditionType::eIsGreaterThanOrEqual:
		return type.f >= aCondition.target;
	case AnimationConditionType::eIsLessThan:
		return type.f < aCondition.target;
	case AnimationConditionType::eIsLessThanOrEqual:
		return type.f <= aCondition.target;
	case AnimationConditionType::eIsTrue:
		return type.b;
	case AnimationConditionType::eIsFalse:
		return !type.b;
	default:
		break;
	}
	return false;
}

void AnimationController::SetDefaultState(const unsigned int& aStateIndex)
{
	myDefaultStateIndex = aStateIndex;
}