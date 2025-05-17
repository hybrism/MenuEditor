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
	Vector2i entityData{ INVALID_ENTITY, -1 }; // x = entityID, y = renderOrder
};

// TODO: find way to use this in instanced data?
struct SkeletalMeshInstanceData : public StaticMeshInstanceData
{
	Pose pose;
};

struct VfxMeshInstanceData
{
	float time = 0.0f;
};

struct ParticleInstanceData
{
	Vector4f color = { 1, 1, 1, 1 };
	float time = 0.0f;
};

// ENUM ALIGNMENT IS NEEDED TO ALIGN WITH THE UNION TYPE
enum class InstanceDataType
{
	SkeletalMesh,
	StaticMesh,
	VfxMesh,
	Particle,
	Count
};

struct MeshDrawerInstanceData
{
	Transform transform{};
	union
	{
		SkeletalMeshInstanceData skeletalMeshData; // largest element
		StaticMeshInstanceData staticMeshData;
		VfxMeshInstanceData vfxMeshData;
		ParticleInstanceData particleData;
	};

	// This needs to be after these elements to enforce block memory layout
	InstanceDataType type = InstanceDataType::Count;

	MeshDrawerInstanceData()
	{
		memset(&skeletalMeshData, 0, sizeof(skeletalMeshData));
	}

	MeshDrawerInstanceData(const Transform& transform, const SkeletalMeshInstanceData& aSkeletalMeshData)
		: transform(transform)
	{
		skeletalMeshData = aSkeletalMeshData;
		type = InstanceDataType::SkeletalMesh;
	}

	MeshDrawerInstanceData(const Transform& transform, const StaticMeshInstanceData& aStaticMeshData)
		: transform(transform)
	{
		memset(&skeletalMeshData, 0, sizeof(skeletalMeshData));
		staticMeshData = aStaticMeshData;
		type = InstanceDataType::StaticMesh;
	}

	MeshDrawerInstanceData(const Transform& transform, const VfxMeshInstanceData& aVfxMeshData)
		: transform(transform)
	{
		memset(&skeletalMeshData, 0, sizeof(skeletalMeshData));
		vfxMeshData = aVfxMeshData;
		type = InstanceDataType::VfxMesh;
	}

	MeshDrawerInstanceData(const Transform& transform, const ParticleInstanceData& aParticleData)
		: transform(transform)
	{
		memset(&skeletalMeshData, 0, sizeof(skeletalMeshData));
		particleData = aParticleData;
		type = InstanceDataType::Particle;
	}
};

//typedef std::variant<
//	VfxMeshInstanceData,
//	ParticleInstanceData,
//	StaticMeshInstanceData,
//	SkeletalMeshInstanceData> MeshDrawerInstanceData;

// TODO: Support different shaders for each mesh
struct MeshInstanceRenderData
{
	MeshDrawerInstanceData data;
	VsType vsType = VsType::DefaultPBR;
	PsType psType = PsType::Count;
	RenderMode renderMode = RenderMode::TRIANGLELIST;
	Transform transform{}; // these are used in sprites for now
};
