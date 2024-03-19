#include "pch.h"

#include "ScriptNodeBase.h"
#include "ScriptNodeTypeRegistry.h"

#include "../ScriptExecutionContext.h"
#include "../ScriptCommon.h"
#include "../ScriptStringRegistry.h"
#include "../ScriptUpdateContext.h"

#include <ecs/World.h>
#include "../game/component/TransformComponent.h"

class EntityNode : public ScriptNodeBase
{
	ScriptPinId myEntityInputPin;
	ScriptPinId myFromPositionPin;

public:
	void Init(const ScriptCreationContext& aContext) override
	{
		{
			ScriptPin entityType = {};
			entityType.dataType = ScriptLinkDataType::Entity;
			entityType.name = ScriptStringRegistry::RegisterOrGetStringId("Entity");
			entityType.node = aContext.GetNodeId();
			entityType.defaultValue.data = INVALID_ENTITY;
			entityType.role = ScriptPinRole::Input;

			myFromPositionPin = aContext.FindOrCreatePin(entityType);
		}

		{
			ScriptPin outputPin = {};
			outputPin.dataType = ScriptLinkDataType::Entity;
			outputPin.name = ScriptStringRegistry::RegisterOrGetStringId("");
			outputPin.node = aContext.GetNodeId();
			outputPin.role = ScriptPinRole::Output;

			myEntityInputPin = aContext.FindOrCreatePin(outputPin);
		}
	}

	ScriptLinkData ReadPin(ScriptExecutionContext& aContext, ScriptPinId) const override
	{
		Entity entity = std::get<Entity>(aContext.ReadInputPin(myFromPositionPin).data);

		return { entity };
	}
};

class GetPositionNode : public ScriptNodeBase
{
	ScriptPinId myEntityInputPin;
	ScriptPinId myPositionOutputPin;

public:
	void Init(const ScriptCreationContext& aContext) override
	{
		{
			ScriptPin entityType = {};
			entityType.dataType = ScriptLinkDataType::Entity;
			entityType.name = ScriptStringRegistry::RegisterOrGetStringId("Entity");
			entityType.node = aContext.GetNodeId();
			entityType.defaultValue.data = INVALID_ENTITY;
			entityType.role = ScriptPinRole::Input;

			myEntityInputPin = aContext.FindOrCreatePin(entityType);
		}

		{
			ScriptPin outputPin = {};
			outputPin.dataType = ScriptLinkDataType::Vector3f;
			outputPin.name = ScriptStringRegistry::RegisterOrGetStringId("Position");
			outputPin.node = aContext.GetNodeId();
			outputPin.defaultValue.data = Vector3f(0.f, 0.f, 0.f);
			outputPin.role = ScriptPinRole::Output;

			myPositionOutputPin = aContext.FindOrCreatePin(outputPin);
		}
	}

	ScriptLinkData ReadPin(ScriptExecutionContext& aContext, ScriptPinId) const override
	{
		Entity entity = std::get<Entity>(aContext.ReadInputPin(myEntityInputPin).data);

		auto& transformComponent = aContext.GetUpdateContext().world->GetComponent<TransformComponent>(entity);

		DirectX::XMMATRIX transform = transformComponent.GetWorldTransform(aContext.GetUpdateContext().world, entity);
		Vector3f position =
		{
			transform.r[3].m128_f32[0],
			transform.r[3].m128_f32[1],
			transform.r[3].m128_f32[2]
		};

		return { position };
	}
};

class GetRotationNode : public ScriptNodeBase
{
	ScriptPinId myEntityInputPin;
	ScriptPinId myRotationOutputPin;

public:
	void Init(const ScriptCreationContext& aContext) override
	{
		{
			ScriptPin entityType = {};
			entityType.dataType = ScriptLinkDataType::Entity;
			entityType.name = ScriptStringRegistry::RegisterOrGetStringId("Entity");
			entityType.node = aContext.GetNodeId();
			entityType.defaultValue.data = INVALID_ENTITY;
			entityType.role = ScriptPinRole::Input;

			myEntityInputPin = aContext.FindOrCreatePin(entityType);
		}

		{
			ScriptPin outputPin = {};
			outputPin.dataType = ScriptLinkDataType::Vector3f;
			outputPin.name = ScriptStringRegistry::RegisterOrGetStringId("Rotation");
			outputPin.node = aContext.GetNodeId();
			outputPin.defaultValue.data = Vector3f(0.f, 0.f, 0.f);
			outputPin.role = ScriptPinRole::Output;

			myRotationOutputPin = aContext.FindOrCreatePin(outputPin);
		}
	}

	ScriptLinkData ReadPin(ScriptExecutionContext& aContext, ScriptPinId) const override
	{
		Entity entity = std::get<Entity>(aContext.ReadInputPin(myEntityInputPin).data);

		auto& transformComponent = aContext.GetUpdateContext().world->GetComponent<TransformComponent>(entity);

		DirectX::XMMATRIX transform = transformComponent.GetWorldTransform(aContext.GetUpdateContext().world, entity);
		
		float pitch = asinf(transform.r[2].m128_f32[1]);
		float yaw = atan2f(transform.r[2].m128_f32[0], transform.r[2].m128_f32[2]);
		float roll = atan2f(transform.r[0].m128_f32[1], transform.r[1].m128_f32[1]);

		Vector3f rotation =
		{
			roll * Rad2Deg,
			pitch * Rad2Deg,
			yaw * Rad2Deg
		};

		return { rotation };
	}
};

void RegisterEntityNodes()
{
	ScriptNodeTypeRegistry::RegisterType<EntityNode>("Entity/GetEntity", "Get all Entities in World");
	ScriptNodeTypeRegistry::RegisterType<GetPositionNode>("Entity/GetPosition", "Get Positon");
	ScriptNodeTypeRegistry::RegisterType<GetRotationNode>("Entity/GetRotation", "Get Rotation");
}