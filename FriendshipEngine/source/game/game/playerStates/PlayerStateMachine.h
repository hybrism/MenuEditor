#pragma once
#include <array>
//#include "PlayerState.h"
#include "PlayerStatesInclude.h"

enum class ePlayerClassStates
{
	Ground,
	Airborne,
	Crouch,
	Slide,
	Wallrun,
	Vault,
	Count
};

class PlayerState;

class PlayerStateMachine
{
public:
	PlayerStateMachine();
	~PlayerStateMachine();

	void Init();
	void SetState(ePlayerClassStates aState, PlayerStateUpdateContext& aContext);
	void Update(PlayerStateUpdateContext& aContext);

	ePlayerClassStates GetCurrentState() const { return myCurrentState; }
private:
	PlayerState* GetState() const;
	std::array<PlayerState*, static_cast<size_t>(ePlayerClassStates::Count)> myStates;
	ePlayerClassStates myCurrentState;
};