#pragma once
#include "PlayerGroundState.h"
#include "PlayerAirbourneState.h"
#include "PlayerCrouchState.h"
#include "PlayerSlideState.h"
#include "PlayerWallrunState.h"
#include "PlayerVaultState.h"


enum class ePlayerClassStates
{
	GroundState,
	AirbourneState,
	CrouchState,
	SlideState,
	WallrunState,
	VaultState,

	Count
};