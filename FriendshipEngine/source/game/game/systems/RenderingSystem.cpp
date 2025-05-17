#include "pch.h"
#include "RenderingSystem.h"
#include "../component/MeshComponent.h"
#include <ecs/World.h>
#include <assets/ModelFactory.h>
#include <iostream>

#include <assets/AssetDatabase.h>
#include <engine/graphics/model/Mesh.h>
#include <engine/graphics/model/SkeletalMesh.h>
#include "../component/TransformComponent.h"
#include "../component/SphereColliderComponent.h"
#include "../component/AnimationDataComponent.h"
#include <engine/graphics\GraphicsEngine.h>
#include <engine/graphics\GBuffer.h>
#include "utility\GameHelperFunctions.h"
//#include <engine/graphics/DirectionalLightManager.h>

#include <engine/graphics/PostProcess.h>
#include <engine/utility/InputManager.h>
#include <assets/ShaderDatabase.h>
#include <engine/shaders/VertexShader.h>
#include <engine/shaders/PixelShader.h>
#include <engine/graphics/model/MeshDrawer.h>

#include <engine/graphics/Camera.h>
#include <engine/debug/DebugLine.h>


RenderingSystem::RenderingSystem(World* aWorld) : System(aWorld)
{
	ComponentSignature signature;
	signature.set(myWorld->GetComponentSignatureID<TransformComponent>());
	signature.set(myWorld->GetComponentSignatureID<MeshComponent>());

	aWorld->SetSystemSignature<RenderingSystem>(signature);
}

RenderingSystem::~RenderingSystem()
{

}

void RenderingSystem::Init()
{

}

void RenderingSystem::Update(SceneUpdateContext&)
{
//#ifndef _RELEASE
	auto* im = InputManager::GetInstance();
	if (im->IsKeyPressed(VK_F6))
	{
		GraphicsEngine::GetInstance()->IncrementRenderMode();
	}
//#endif
}

void RenderingSystem::Render()
{
	auto* ge = GraphicsEngine::GetInstance();
	

	auto& deferredRenderer = ge->GetDeferredRenderer();

	myFrustum.SetFarAndNearPlanes();

	for (auto& entity : myEntities)
	{
		auto& meshComponent = myWorld->GetComponent<MeshComponent>(entity);

		if (!meshComponent.shouldRender) { continue; }

		if (CullObject(entity))
		{
			continue;
		}

		size_t meshId = meshComponent.id;
		auto& meshes = AssetDatabase::GetMesh(meshId);

		size_t size = meshes.meshData.size();
		for (size_t i = 0; i < size; ++i)
		{
			auto& mesh = meshes.meshData[i];
			auto& transformComponent = myWorld->GetComponent<TransformComponent>(entity);

			Transform hierarchyTransform = transformComponent.GetWorldTransform(myWorld, entity);

			if (meshComponent.type == MeshType::Skeletal)
			{
				auto& anim = myWorld->GetComponent<AnimationDataComponent>(entity);

				Transform transform = hierarchyTransform;
				transform *= meshComponent.offset;

				SkeletalMeshInstanceData instanceData;
				instanceData.entityData = Vector2i{ static_cast<int>(entity), meshComponent.vertexPaintedIndex };
				instanceData.pose = anim.localSpacePose;

				deferredRenderer.Render(
					static_cast<SkeletalMesh*>(mesh),
					MeshInstanceRenderData{
						MeshDrawerInstanceData(transform, instanceData),
						mesh->GetVertexShader()->GetType(),
						mesh->GetPixelShader()->GetType(),
						RenderMode::TRIANGLELIST
					},
					meshComponent.shouldDisregardDepth
				);
				continue;
			}

			StaticMeshInstanceData instanceData;
			instanceData.entityData = Vector2i{ static_cast<int>(entity), meshComponent.vertexPaintedIndex };

			Transform transform = hierarchyTransform;
			transform *= meshComponent.offset;

			deferredRenderer.Render(
				static_cast<Mesh*>(mesh),
				{
					MeshDrawerInstanceData(transform, instanceData),
					mesh->GetVertexShader()->GetType(),
					mesh->GetPixelShader()->GetType(),
					RenderMode::TRIANGLELIST,
				}
			);
		}
	}
}

bool RenderingSystem::CullObject(const Entity& aEntity)
{
	

	if (myWorld->HasComponent<SphereColliderComponent>(aEntity))
	{
		SphereColliderComponent& sphereComp = myWorld->GetComponent<SphereColliderComponent>(aEntity);
		BoundingCullSphere sphere(sphereComp.center, sphereComp.radius);

		return myFrustum.CullMesh(sphere);
	}

	return false;
}

