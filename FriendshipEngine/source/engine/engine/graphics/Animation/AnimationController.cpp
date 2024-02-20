#include "pch.h"
#include "AnimationController.h"
#include <assets/AssetDatabase.h>

AnimationController::AnimationController(AssetDatabase* aAssetDatabase, const size_t& aMeshId)
{
	myAssetDatabase = aAssetDatabase;
	myMeshId = aMeshId;
}

AnimationController::~AnimationController()
{
	myAnimationStates.clear();
	myParameterNameToIndexMap.clear();
	myAssetDatabase = nullptr;
}

void AnimationController::Update(const float& aDeltaTime, AnimationData& aData)
{
	aData.previousStateIndex = aData.currentStateIndex;
	if (aData.transitionIndex == -1) { return; }

	aData.exitTimer += aDeltaTime;
	auto* transition = GetCurrentState(aData).GetTransition(aData.transitionIndex);

	if (transition == nullptr) { return; }

	if (aData.exitTimer < transition->exitTime) { return; }

	aData.transitionTimer += aDeltaTime;

	if (aData.transitionTimer < transition->duration) { return; }
	//{
	//	Animation* animation = myAssetDatabase->GetAnimation(myMeshId, myCurrentAnimationIndex);
	//	bool hasTimeExceededFixedDuration = myAnimationData->time < animation->Duration * transition.duration;
	//	if (transition.fixedDuration && hasTimeExceededFixedDuration) { return; }
	//	else if (myTransitionTimer < transition.duration) { return; }
	//}
	aData.currentStateIndex = aData.nextStateIndex;
	aData.transitionTimer = 0.0f;
	aData.exitTimer = 0.0f;
	aData.transitionIndex = -1;


	//myStates[myCurrentAnimation].Update(aDeltaTime);
}

size_t AnimationController::AddState(const int& aAnimationIndex)
{
	return AddState(AnimationState(aAnimationIndex));
}

size_t AnimationController::AddState(const AnimationState& aAnimationState)
{
	size_t index = myAnimationStates.size();

	myAnimationStates.push_back(aAnimationState);

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
	bool shouldTransition = false;
	for (size_t i = 0; i < animationCount; i++)
	{
		const AnimationTransition* transition = currentState.GetTransition(i);

		for (size_t j = 0; j < transition->conditions.size(); j++)
		{
			const AnimationCondition& condition = transition->conditions[j];
			if (IsAnimationConditionMet(condition.parameterIndex, condition, aParameter))
			{
				shouldTransition = true;
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
		return type.i == 1;
	case AnimationConditionType::eIsFalse:
		return type.i == 0;
	default:
		break;
	}
	return false;
}

void AnimationController::SetDefaultState(const unsigned int& aStateIndex)
{
	myDefaultStateIndex = aStateIndex;
}