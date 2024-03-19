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

RenderingSystem::RenderingSystem(World* aWorld) : System(aWorld)
{

}

RenderingSystem::~RenderingSystem()
{

}

void RenderingSystem::Init()
{
	myMeshOrderCounter.resize(AssetDatabase::GetMeshCount(), std::vector<unsigned int>(0));
}

void RenderingSystem::Update(const SceneUpdateContext&)
{
#ifndef _RELEASE
	auto* im = InputManager::GetInstance();
	if (im->IsKeyPressed(VK_F6))
	{
		GraphicsEngine::GetInstance()->IncrementRenderMode();
	}
#endif
}

void RenderingSystem::Render()
{
	auto* ge = GraphicsEngine::GetInstance();
	//{
	//	//auto* lightManager = ge->GetDirectionalLightManager();

	//	if (myWorld->GetPlayerEntityID() != INVALID_ENTITY)
	//	{
	//		auto& playerTransformComponent = myWorld->GetComponent<TransformComponent>(myWorld->GetPlayerEntityID());

	//		//lightManager->SetDirCamerasPos(playerTransformComponent.transform.GetPosition());
	//	}	
	//}

	auto& deferredRenderer = ge->GetDeferredRenderer();
	//auto& forwardRenderer = ge->GetForwardRenderer();

	for (auto& entity : myEntities)
	{
		auto& meshComponent = myWorld->GetComponent<MeshComponent>(entity);

		if (!meshComponent.shouldRender) { continue; }

		size_t meshId = meshComponent.id;
		auto& meshes = AssetDatabase::GetMesh(meshId);

		size_t size = meshes.meshData.size();
		for (size_t i = 0; i < size; ++i)
		{
			auto& mesh = meshes.meshData[i];

			if (myMeshOrderCounter.size() < meshId)
			{
				myMeshOrderCounter.resize(meshId + 1, std::vector<unsigned int>(size));
			}
			else if (myMeshOrderCounter[meshId].size() < size)
			{
				myMeshOrderCounter[meshId].resize(size);
			}

			meshComponent.renderOrder = myMeshOrderCounter[meshId][i];
			++myMeshOrderCounter[meshId][i];

			auto& transformComponent = myWorld->GetComponent<TransformComponent>(entity);

			Transform hierarchyTransform = transformComponent.GetWorldTransform(myWorld, entity);

			if (meshComponent.type == MeshType::Skeletal)
			{
				auto& anim = myWorld->GetComponent<AnimationDataComponent>(entity);

				SkeletalMeshInstanceData instanceData;
				instanceData.entityData = Vector2ui{ static_cast<unsigned int>(entity), 0 };
				instanceData.pose = anim.localSpacePose;
				instanceData.transform = hierarchyTransform;
				instanceData.transform *= meshComponent.offset;

				deferredRenderer.Render(
					static_cast<SkeletalMesh*>(mesh),
					MeshInstanceRenderData{
						instanceData,
						mesh->GetVertexShader()->GetType(),
						mesh->GetPixelShader()->GetType(),
						RenderMode::TRIANGLELIST
					},
					meshComponent.shouldDisregardDepth
				);
				continue;
			}

			StaticMeshInstanceData instanceData;
			instanceData.entityData = Vector2ui{ static_cast<unsigned int>(entity), 0 };
			instanceData.transform = hierarchyTransform;
			instanceData.transform *= meshComponent.offset;

			deferredRenderer.Render(
				static_cast<Mesh*>(mesh),
				{
					instanceData,
					mesh->GetVertexShader()->GetType(),
					mesh->GetPixelShader()->GetType(),
					RenderMode::TRIANGLELIST,
				}
			);
		}
	}

	for (size_t i = 0; i < myMeshOrderCounter.size(); ++i)
	{
		std::fill(myMeshOrderCounter[i].begin(), myMeshOrderCounter[i].end(), 0);
	}
}

