#pragma once
#include "../../math/Transform.h"
#include "../animation/Pose.h"
#include "../RenderDefines.h"
#include <assets/ShaderTypeEnum.h>
#include <variant>

struct VfxMeshInstanceData
{
	float time = 0.0f;
};

struct StaticMeshInstanceData
{
	Transform transform;
};

// TODO: find way to use this in instanced data?
struct SkeletalMeshInstanceData : public StaticMeshInstanceData
{
	Pose pose;
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
	Transform transform{};
};
