#pragma once
#include "../../math/Transform.h"
#include "../animation/Pose.h"
#include "../RenderDefines.h"
#include <ecs/entity/Entity.h>
#include <assets/ShaderTypeEnum.h>
#include <variant>

// THESE ARE USED IN THE VERTEX SHADERS SENT IN BY THE MESH DRAWER
// TO SEE THE CORRESPONDING SHADER INFO, LOOK IN COMMON.HLSLI

struct StaticMeshInstanceData
{
	Transform transform{};
	Vector2ui entityData{ INVALID_ENTITY, 0 }; // x = entityID, y = renderOrder
};

// TODO: find way to use this in instanced data?
struct SkeletalMeshInstanceData : public StaticMeshInstanceData
{
	Pose pose;
};

struct VfxMeshInstanceData
{
	Transform transform{};
	float time = 0.0f;
};

typedef std::variant<
	VfxMeshInstanceData,
	StaticMeshInstanceData,
	SkeletalMeshInstanceData> MeshDrawerInstanceData;

// TODO: Support different shaders for each mesh
struct MeshInstanceRenderData
{
	MeshDrawerInstanceData data;
	VsType vsType = VsType::DefaultPBR;
	PsType psType = PsType::Count;
	RenderMode renderMode = RenderMode::TRIANGLELIST;
	Transform transform{}; // these are used in sprites for now
};
