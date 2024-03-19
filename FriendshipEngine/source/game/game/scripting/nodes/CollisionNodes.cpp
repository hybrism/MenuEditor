#include "pch.h"

#include "ScriptNodeBase.h"
#include "ScriptNodeTypeRegistry.h"

#include "../ScriptExecutionContext.h"
#include "../ScriptCommon.h"
#include "../ScriptStringRegistry.h"
#include "../ScriptUpdateContext.h"

#include "../../component/CollisionDataComponent.h"

class OverlapStart : public ScriptNodeBase
{
	class State : public ScriptNodeRuntimeInstanceBase
	{
	public:
		bool hasExecutedOnce = false;
	};

	ScriptPinId myInputFlowPin;
	ScriptPinId myFirstEntityPin;
	ScriptPinId mySecondEntityPin;
	ScriptPinId myOutputFlowPin;

public:

	void Init(const ScriptCreationContext& aContext) override
	{
		{
			ScriptPin inputFlow = {};
			inputFlow.dataType = ScriptLinkDataType::Flow;
			inputFlow.name = ScriptStringRegistry::RegisterOrGetStringId("Run");
			inputFlow.node = aContext.GetNodeId();
			inputFlow.role = ScriptPinRole::Input;

			myInputFlowPin = aContext.FindOrCreatePin(inputFlow);
		}

		{
			ScriptPin entity = {};
			entity.dataType = ScriptLinkDataType::Entity;
			entity.name = ScriptStringRegistry::RegisterOrGetStringId("Entity first");
			entity.node = aContext.GetNodeId();
			entity.defaultValue.data = INVALID_ENTITY;
			entity.role = ScriptPinRole::Input;

			mySecondEntityPin = aContext.FindOrCreatePin(entity);
		}

		{
			ScriptPin entity = {};
			entity.dataType = ScriptLinkDataType::Entity;
			entity.name = ScriptStringRegistry::RegisterOrGetStringId("Entity second");
			entity.node = aContext.GetNodeId();
			entity.defaultValue.data = INVALID_ENTITY;
			entity.role = ScriptPinRole::Input;

			myFirstEntityPin = aContext.FindOrCreatePin(entity);
		}

		{
			ScriptPin outputFlow = {};
			outputFlow.dataType = ScriptLinkDataType::Flow;
			outputFlow.name = ScriptStringRegistry::RegisterOrGetStringId("Run");
			outputFlow.node = aContext.GetNodeId();
			outputFlow.role = ScriptPinRole::Output;

			myOutputFlowPin = aContext.FindOrCreatePin(outputFlow);
		}
	}

	std::unique_ptr<ScriptNodeRuntimeInstanceBase> CreateRuntimeInstanceData() const { return std::make_unique<State>(); }

	ScriptNodeResult Execute(ScriptExecutionContext& aContext, ScriptPinId) const override
	{
		State* state = static_cast<State*>(aContext.GetRuntimeInstanceData());

		if (state->hasExecutedOnce)
			return ScriptNodeResult::Finished;

		Entity first = std::get<Entity>(aContext.ReadInputPin(myFirstEntityPin).data);
		Entity second = std::get<Entity>(aContext.ReadInputPin(mySecondEntityPin).data);

		World* world = aContext.GetUpdateContext().world;

		auto& secondCollider = world->GetComponent<CollisionDataComponent>(second);
		auto& firstCollider = world->GetComponent<CollisionDataComponent>(first);

		if (firstCollider.isColliding && secondCollider.isColliding)
		{
			aContext.TriggerOutputPin(myOutputFlowPin);
			state->hasExecutedOnce = true;
		}

		return ScriptNodeResult::Finished;
	}
};

void RegisterCollisionNodes()
{
	ScriptNodeTypeRegistry::RegisterType<OverlapStart>("Collision/OnOverlapBegin", "A node that executes once when the overlap begins");
}