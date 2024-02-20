#include "pch.h"
#include "StateMachine.h"
#include "State.h"

StateMachine::StateMachine()
{
	myCurrentState = nullptr;
}

StateMachine::~StateMachine()
{
	if (myCurrentState != nullptr)
	{
		myCurrentState->OnExit();
	}
}

void StateMachine::SetState(State* aState)
{
	if (myCurrentState != nullptr)
	{
		myCurrentState->OnExit();
	}

	myCurrentState = aState;
	myCurrentState->OnEnter();
}

void StateMachine::Update(const float& dt)
{
	if (myCurrentState == nullptr) { return; }

	myCurrentState->Update(dt);
}

void StateMachine::Render()
{
	myCurrentState->Render();
}

State* StateMachine::GetCurrentState() const
{
	return myCurrentState;
}
