#pragma once
#include <string>

enum class AnimationConditionType
{
	eIsEqual,
	eIsNotEqual,
	eIsGreaterThan,
	eIsGreaterThanOrEqual,
	eIsLessThan,
	eIsLessThanOrEqual,
	eIsTrue,
	eIsFalse
};

struct AnimationCondition
{
	size_t parameterIndex;
	float target;
	AnimationConditionType type;

	AnimationCondition(
		const size_t& aParameterIndex,
		const float& aComparisonValue,
		const AnimationConditionType& aType
	) : parameterIndex(aParameterIndex), target(aComparisonValue), type(aType)
	{ }

	AnimationCondition(
		const size_t & aParameterIndex,
		const AnimationConditionType & aType
	) : parameterIndex(aParameterIndex), target(0), type(aType)
	{ }
};

struct AnimationTransition
{
	//int fromStateIndex = -1;
	int toStateIndex = -1;
	float exitTime = 0;
	float duration = 0; // if fixedDuration is true this is used as seconds, else as a % of the total animation time
	float transitionOffset = 0;
	bool fixedDuration = true;
	bool isInterruptable = true;

	std::vector<AnimationCondition> conditions;
};

class AnimationState
{
	friend class AnimationController;
public:
	AnimationState(const std::string& aAnimationName);
	AnimationState(const int& aAnimationIndex) : animationIndex(aAnimationIndex) { }

	void AddTransition(AnimationTransition aTransition)
	{
		transitions.push_back(aTransition);
	}
	void RemoveTransition(const size_t& aTransitionIndex) { transitions.erase(transitions.begin() + aTransitionIndex); }
	
	void SetSpeed(const float& aSpeed) { speed = aSpeed; }
	void SetLooping(const bool& aLooping) { looping = aLooping; }

	size_t GetTransitionCount() const { return transitions.size(); }
	int GetAnimationIndex() const { return animationIndex; }
	float GetSpeed() const { return speed; }
	bool IsLooping() const { return looping; }
	AnimationTransition* GetTransition(const size_t& aTransitionIndex)
	{
		if (aTransitionIndex >= transitions.size())
			return nullptr;
		return &transitions[aTransitionIndex];
	}
private:
#ifdef _EDITOR
	int stateIndex = -1;
#endif
	int animationIndex = -1;
	float speed = 1.0f;
	bool looping = true;

	std::vector<AnimationTransition> transitions = {};
};
