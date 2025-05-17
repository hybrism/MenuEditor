#pragma once
#include <vector>
#include <variant>
#include <string>
#include <cassert>
#include <unordered_map>

#include "AnimationStructs.h"
#include "AnimationState.h"

class AssetDatabase;
struct Animation;


// TODO: Make this pure so that we can store one controller per mesh and make it interact with the ECS

// Holds information about current animation state, and next if transitioning
class AnimationController
{
public:
	AnimationController(AssetDatabase* aAssetDatabase, int aId);
	~AnimationController();

	void Update(const float& aDeltaTime, Animation& aAnimation, AnimationData& aData);
	void AddParameter(const std::string& aName, const size_t& aIndex);

	void SetParameter(
		const size_t& aIndex,
		const AnimationDataParameter& aValue,
		AnimationData& aData,
		AnimationDataParameterContainer& aParameter
	);
	void SetParameter(
		const std::string& aName,
		const AnimationDataParameter& aValue,
		AnimationData& aData,
		AnimationDataParameterContainer& aParameter
	);

	AnimationDataParameter GetParameter(const size_t& aIndex, AnimationDataParameterContainer& aParameter) const;
	AnimationDataParameter GetParameter(const std::string& aName, AnimationDataParameterContainer& aParameter) const;
	size_t GetParameterIndexFromName(const std::string& aName) const { return myParameterNameToIndexMap.at(aName); }
	size_t AddState(const int& aAnimationIndex);
	size_t AddState(const AnimationState& aAnimationState);
	AnimationState& GetState(const size_t& aStateIndex);
	AnimationState& GetCurrentState(AnimationData& aData);
	void SetState(const unsigned int& aStateIndex, AnimationData& aData);
	void SetDefaultState(const unsigned int& aStateIndex);

	bool HasAnimationStates() const { return myAnimationStates.size() > 0; }
	int GetCurrentAnimationIndex(AnimationData& aData);
	unsigned int GetDefaultStateIndex() const { return myDefaultStateIndex; }

	int GetId() const { return myId; }
private:
	void RunTransitionCheck(AnimationData& aData, AnimationDataParameterContainer& aParameter);
	bool IsAnimationConditionMet(const size_t& aParameterIndex, const AnimationCondition& aCondition, AnimationDataParameterContainer& aParameter) const;

	std::vector<AnimationState> myAnimationStates;
	std::unordered_map<std::string, size_t> myParameterNameToIndexMap;

	unsigned int myDefaultStateIndex = 0;
	int myId = 0;
	AssetDatabase* myAssetDatabase = nullptr;
};

inline void AnimationController::AddParameter(const std::string& aName, const size_t& aIndex)
{
	assert(myParameterNameToIndexMap.find(aName) == myParameterNameToIndexMap.end() && "Parameter with name already exists");
	myParameterNameToIndexMap[aName] = aIndex;
}

inline void AnimationController::SetParameter(
	const size_t& aIndex,
	const AnimationDataParameter& aValue,
	AnimationData& aData,
	AnimationDataParameterContainer& aParameter
)
{
	assert(aIndex < ANIMATION_DATA_PARAMETER_COUNT && "Index out of range");
	
	AnimationDataParameter& parameter = aParameter[aIndex];
	parameter = aValue;

	RunTransitionCheck(aData, aParameter);
}

inline void AnimationController::SetParameter(
	const std::string& aName,
	const AnimationDataParameter& aValue,
	AnimationData& aData,
	AnimationDataParameterContainer& aParameter)
{
	assert(myParameterNameToIndexMap.find(aName) != myParameterNameToIndexMap.end() && "Parameter with name does not exist");
	SetParameter(myParameterNameToIndexMap.at(aName), aValue, aData, aParameter);
}

inline AnimationDataParameter AnimationController::GetParameter(const size_t& aIndex, AnimationDataParameterContainer& aParameter) const
{
	assert(aIndex < ANIMATION_DATA_PARAMETER_COUNT && "Index out of range");
	return aParameter[aIndex];
}

inline AnimationDataParameter AnimationController::GetParameter(const std::string& aName, AnimationDataParameterContainer& aParameter) const
{
	assert(myParameterNameToIndexMap.find(aName) != myParameterNameToIndexMap.end() && "Parameter with name does not exist");
	return GetParameter(myParameterNameToIndexMap.at(aName), aParameter);
}
