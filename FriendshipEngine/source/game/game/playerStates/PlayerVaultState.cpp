#include "pch.h"
#include "PlayerVaultState.h"

void PlayerVaultState::OnEnter()
{
	Error::DebugPrintString("Vault Enter");
}

void PlayerVaultState::OnExit()
{
	Error::DebugPrintString("Vault Exit");
}

void PlayerVaultState::Update(const float& dt)
{
	dt;
}
