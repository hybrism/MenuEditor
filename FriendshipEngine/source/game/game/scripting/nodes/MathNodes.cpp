#include "pch.h"

#include "ScriptNodeBase.h"
#include "ScriptNodeTypeRegistry.h"

#include "../ScriptExecutionContext.h"
#include "../ScriptCommon.h"
#include "../ScriptStringRegistry.h"
#include "../ScriptUpdateContext.h"

#include <ecs/World.h>
#include "../game/component/TransformComponent.h"
#include <engine/math/helper.h>

class PositionLerpNode : public ScriptNodeBase
{
	class State : public ScriptNodeRuntimeInstanceBase
	{
	public:
		Vector3f startPosition;
		Vector3f endPosition;
		bool isValueSet = false;
		float journeyLength = 0;
		float timer = 0;
	};

	ScriptPinId myInputFlowPin;
	ScriptPinId myOutputFlowPin;
	ScriptPinId myDurationPin;
	ScriptPinId myEntityToMovePin;
	ScriptPinId myFromPositionPin;
	ScriptPinId myToPositionPin;

public:
	void Init(const ScriptCreationContext& aContext) override
	{
		{
			ScriptPin inputFlow = {};
			inputFlow.dataType = ScriptLinkDataType::Flow;
			inputFlow.name = ScriptStringRegistry::RegisterOrGetStringId("");
			inputFlow.node = aContext.GetNodeId();
			inputFlow.role = ScriptPinRole::Input;

			myInputFlowPin = aContext.FindOrCreatePin(inputFlow);
		}

		{
			ScriptPin outputFlow = {};
			outputFlow.dataType = ScriptLinkDataType::Flow;
			outputFlow.name = ScriptStringRegistry::RegisterOrGetStringId("Run");
			outputFlow.node = aContext.GetNodeId();
			outputFlow.role = ScriptPinRole::Output;

			myOutputFlowPin = aContext.FindOrCreatePin(outputFlow);
		}

		{
			ScriptPin entityType = {};
			entityType.dataType = ScriptLinkDataType::Entity;
			entityType.name = ScriptStringRegistry::RegisterOrGetStringId("Entity");
			entityType.node = aContext.GetNodeId();
			entityType.defaultValue.data = INVALID_ENTITY;
			entityType.role = ScriptPinRole::Input;

			myEntityToMovePin = aContext.FindOrCreatePin(entityType);
		}

		{
			ScriptPin durationPin = {};
			durationPin.dataType = ScriptLinkDataType::Float;
			durationPin.name = ScriptStringRegistry::RegisterOrGetStringId("Time");
			durationPin.node = aContext.GetNodeId();
			durationPin.defaultValue.data = 1.f;
			durationPin.role = ScriptPinRole::Input;

			myDurationPin = aContext.FindOrCreatePin(durationPin);
		}

		{
			ScriptPin inputPosition = {};
			inputPosition.dataType = ScriptLinkDataType::Vector3f;
			inputPosition.name = ScriptStringRegistry::RegisterOrGetStringId("From Position");
			inputPosition.node = aContext.GetNodeId();
			inputPosition.defaultValue.data = Vector3f(0.f, 0.f, 0.f);
			inputPosition.role = ScriptPinRole::Input;

			myFromPositionPin = aContext.FindOrCreatePin(inputPosition);
		}

		{
			ScriptPin outputPin = {};
			outputPin.dataType = ScriptLinkDataType::Vector3f;
			outputPin.name = ScriptStringRegistry::RegisterOrGetStringId("To Position");
			outputPin.node = aContext.GetNodeId();
			outputPin.defaultValue.data = Vector3f(0.f, 0.f, 0.f);
			outputPin.role = ScriptPinRole::Input;

			myToPositionPin = aContext.FindOrCreatePin(outputPin);
		}
	}

	std::unique_ptr<ScriptNodeRuntimeInstanceBase> CreateRuntimeInstanceData() const { return std::make_unique<State>(); }

	ScriptNodeResult Execute(ScriptExecutionContext& aContext, ScriptPinId) const override
	{
		State* state = static_cast<State*>(aContext.GetRuntimeInstanceData());
		Entity entity = std::get<Entity>(aContext.ReadInputPin(myEntityToMovePin).data);
		Vector3f currentPosition = std::get<Vector3f>(aContext.ReadInputPin(myFromPositionPin).data);
		Vector3f toPosition = std::get<Vector3f>(aContext.ReadInputPin(myToPositionPin).data);
		const float speed = std::get<float>(aContext.ReadInputPin(myDurationPin).data);
		auto& transformComponent = aContext.GetUpdateContext().world->GetComponent<TransformComponent>(entity);

		if (!state->isValueSet)
		{
			state->startPosition = currentPosition;
			state->endPosition = toPosition;
			state->journeyLength = Vector3f::Distance(state->startPosition, state->endPosition);
			state->timer = 0;
			state->isValueSet = true;
		}

		state->timer += aContext.GetUpdateContext().deltaTime;

		float distanceCovered = (state->timer - 0) * speed;
		float fractionOfJourney = distanceCovered / state->journeyLength;

		currentPosition.x = FriendMath::Lerp(state->startPosition.x, state->endPosition.x, fractionOfJourney);
		currentPosition.y = FriendMath::Lerp(state->startPosition.y, state->endPosition.y, fractionOfJourney);
		currentPosition.z = FriendMath::Lerp(state->startPosition.z, state->endPosition.z, fractionOfJourney);


		if (Vector3f::Distance(state->endPosition, transformComponent.transform.GetPosition()) < 1.f)
		{
			aContext.TriggerOutputPin(myOutputFlowPin);
			transformComponent.transform.SetPosition(state->endPosition);
			return ScriptNodeResult::Finished;
		}

		transformComponent.transform.SetPosition(currentPosition);
		return ScriptNodeResult::KeepRunning;
	}
};

class RotationLerpNode : public ScriptNodeBase
{
	class State : public ScriptNodeRuntimeInstanceBase
	{
	public:
		Vector3f startRotation;
		Vector3f finalRotation;
		bool isValueSet = false;
		float timer = 0;
	};

	ScriptPinId myInputFlowPin;
	ScriptPinId myOutputFlowPin;
	ScriptPinId myDurationPin;
	ScriptPinId myEntityToMovePin;
	ScriptPinId myFromRotationPin;
	ScriptPinId myToRotationPin;

public:
	void Init(const ScriptCreationContext& aContext) override
	{
		{
			ScriptPin inputFlow = {};
			inputFlow.dataType = ScriptLinkDataType::Flow;
			inputFlow.name = ScriptStringRegistry::RegisterOrGetStringId("");
			inputFlow.node = aContext.GetNodeId();
			inputFlow.role = ScriptPinRole::Input;

			myInputFlowPin = aContext.FindOrCreatePin(inputFlow);
		}

		{
			ScriptPin outputFlow = {};
			outputFlow.dataType = ScriptLinkDataType::Flow;
			outputFlow.name = ScriptStringRegistry::RegisterOrGetStringId("Run");
			outputFlow.node = aContext.GetNodeId();
			outputFlow.role = ScriptPinRole::Output;

			myOutputFlowPin = aContext.FindOrCreatePin(outputFlow);
		}

		{
			ScriptPin entityType = {};
			entityType.dataType = ScriptLinkDataType::Entity;
			entityType.name = ScriptStringRegistry::RegisterOrGetStringId("Entity");
			entityType.node = aContext.GetNodeId();
			entityType.defaultValue.data = INVALID_ENTITY;
			entityType.role = ScriptPinRole::Input;

			myEntityToMovePin = aContext.FindOrCreatePin(entityType);
		}

		{
			ScriptPin durationPin = {};
			durationPin.dataType = ScriptLinkDataType::Float;
			durationPin.name = ScriptStringRegistry::RegisterOrGetStringId("Time");
			durationPin.node = aContext.GetNodeId();
			durationPin.defaultValue.data = 1.f;
			durationPin.role = ScriptPinRole::Input;

			myDurationPin = aContext.FindOrCreatePin(durationPin);
		}

		{
			ScriptPin inputPosition = {};
			inputPosition.dataType = ScriptLinkDataType::Vector3f;
			inputPosition.name = ScriptStringRegistry::RegisterOrGetStringId("From Position");
			inputPosition.node = aContext.GetNodeId();
			inputPosition.defaultValue.data = Vector3f(0.f, 0.f, 0.f);
			inputPosition.role = ScriptPinRole::Input;

			myFromRotationPin = aContext.FindOrCreatePin(inputPosition);
		}

		{
			ScriptPin outputPin = {};
			outputPin.dataType = ScriptLinkDataType::Vector3f;
			outputPin.name = ScriptStringRegistry::RegisterOrGetStringId("To Position");
			outputPin.node = aContext.GetNodeId();
			outputPin.defaultValue.data = Vector3f(0.f, 0.f, 0.f);
			outputPin.role = ScriptPinRole::Input;

			myToRotationPin = aContext.FindOrCreatePin(outputPin);
		}
	}

	std::unique_ptr<ScriptNodeRuntimeInstanceBase> CreateRuntimeInstanceData() const { return std::make_unique<State>(); }

	ScriptNodeResult Execute(ScriptExecutionContext& aContext, ScriptPinId) const override
	{
		State* state = static_cast<State*>(aContext.GetRuntimeInstanceData());
		Entity entity = std::get<Entity>(aContext.ReadInputPin(myEntityToMovePin).data);
		Vector3f currentRotation = std::get<Vector3f>(aContext.ReadInputPin(myFromRotationPin).data);
		const Vector3f finalRotation = std::get<Vector3f>(aContext.ReadInputPin(myToRotationPin).data);
		const float duration = std::get<float>(aContext.ReadInputPin(myDurationPin).data);
		auto& transformComponent = aContext.GetUpdateContext().world->GetComponent<TransformComponent>(entity);

		if (!state->isValueSet)
		{
			state->startRotation = currentRotation;
			state->finalRotation = finalRotation;
			state->timer = 0;
			state->isValueSet = true;
		}

		state->timer += aContext.GetUpdateContext().deltaTime;

		float ratio = state->timer / duration;

		//if (state->timer > duration)
		//	ratio = 1.f;

		currentRotation.x = FriendMath::Lerp(state->startRotation.x, state->finalRotation.x, ratio);
		currentRotation.y = FriendMath::Lerp(state->startRotation.y, state->finalRotation.y, ratio);
		currentRotation.z = FriendMath::Lerp(state->startRotation.z, state->finalRotation.z, ratio);

		if (Vector3f::Distance(currentRotation, state->finalRotation) < 0.5f)
		{
			aContext.TriggerOutputPin(myOutputFlowPin);
			transformComponent.transform.SetEulerAngles(state->finalRotation);
			return ScriptNodeResult::Finished;
		}

		transformComponent.transform.SetEulerAngles(currentRotation);
		return ScriptNodeResult::KeepRunning;
	}
};

class AddToVectorNode : public ScriptNodeBase
{
	ScriptPinId myFirstInputVectorPin;
	ScriptPinId mySecondInputVectorPin;
	ScriptPinId myOutputVectorPin;

public:
	void Init(const ScriptCreationContext& aContext) override
	{
		{
			ScriptPin inputPosition = {};
			inputPosition.dataType = ScriptLinkDataType::Vector3f;
			inputPosition.name = ScriptStringRegistry::RegisterOrGetStringId("From Position");
			inputPosition.node = aContext.GetNodeId();
			inputPosition.defaultValue.data = Vector3f(0.f, 0.f, 0.f);
			inputPosition.role = ScriptPinRole::Input;

			myFirstInputVectorPin = aContext.FindOrCreatePin(inputPosition);
		}

		{
			ScriptPin inputPosition = {};
			inputPosition.dataType = ScriptLinkDataType::Vector3f;
			inputPosition.name = ScriptStringRegistry::RegisterOrGetStringId("Add");
			inputPosition.node = aContext.GetNodeId();
			inputPosition.defaultValue.data = Vector3f(0.f, 0.f, 0.f);
			inputPosition.role = ScriptPinRole::Input;

			mySecondInputVectorPin = aContext.FindOrCreatePin(inputPosition);
		}

		{
			ScriptPin outputPin = {};
			outputPin.dataType = ScriptLinkDataType::Vector3f;
			outputPin.name = ScriptStringRegistry::RegisterOrGetStringId("");
			outputPin.node = aContext.GetNodeId();
			outputPin.defaultValue.data = Vector3f(0.f, 0.f, 0.f);
			outputPin.role = ScriptPinRole::Output;

			myOutputVectorPin = aContext.FindOrCreatePin(outputPin);
		}
	}

	ScriptLinkData ReadPin(ScriptExecutionContext& aContext, ScriptPinId) const override
	{
		Vector3f firstInput = std::get<Vector3f>(aContext.ReadInputPin(myFirstInputVectorPin).data);
		Vector3f secondInput = std::get<Vector3f>(aContext.ReadInputPin(mySecondInputVectorPin).data);

		return { firstInput + secondInput };
	}
};

void RegisterMathNodes()
{
	ScriptNodeTypeRegistry::RegisterType<PositionLerpNode>("Math/LerpPosition", "Set Position of selected entity");
	ScriptNodeTypeRegistry::RegisterType<RotationLerpNode>("Math/LerpRotation", "Set Rotation of selected entity");
	ScriptNodeTypeRegistry::RegisterType<AddToVectorNode>("Math/AddToVector", "Add value to existing vector");
}