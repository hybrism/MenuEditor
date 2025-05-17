#pragma once

struct PlayerComponent;
struct TransformComponent;
struct AnimationDataComponent;
class PlayerStateMachine;
class PhysXSceneManager;
class AnimationController;

struct PlayerStateUpdateContext
{
	float deltaTime;
	PlayerComponent& playerComponent;
	TransformComponent& transformComponent;
	AnimationDataComponent& animationDataComponent;
	PhysXSceneManager* physXSceneManager;
	AnimationController& animationController;
};

class PlayerState
{
public:
	PlayerState(PlayerStateMachine* aStateMachine) { myStateMachine = aStateMachine; };
	virtual ~PlayerState() = default;

	virtual void OnEnter(PlayerStateUpdateContext& aContext) = 0;
	virtual void OnExit(PlayerStateUpdateContext& aContext) = 0;
	virtual void Update(PlayerStateUpdateContext& aContext) = 0;
protected:
	PlayerStateMachine* myStateMachine = nullptr;
};
