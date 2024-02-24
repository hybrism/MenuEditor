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

void RenderingSystem::Update(const float&)
{
#ifdef _DEBUG
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
		auto& meshes = AssetDatabase::GetMesh(meshComponent.id);

		for (auto& mesh : meshes.meshData)
		{
			Transform hierarchyTransform = GetWorldTransform(myWorld,entity);

			if (meshComponent.type == MeshType::Skeletal)
			{
				auto& anim = myWorld->GetComponent<AnimationDataComponent>(entity);

				deferredRenderer.Render(
					static_cast<SkeletalMesh*>(mesh),
					MeshInstanceRenderData{
						SkeletalMeshInstanceData { hierarchyTransform, anim.localSpacePose },
						mesh->GetVertexShader()->GetType(),
						mesh->GetPixelShader()->GetType(),
						RenderMode::TRIANGLELIST
					},
					meshComponent.shouldDisregardDepth
				);
				continue;
			}

			deferredRenderer.Render(
				static_cast<Mesh*>(mesh),
				{
					StaticMeshInstanceData{ hierarchyTransform },
					mesh->GetVertexShader()->GetType(),
					mesh->GetPixelShader()->GetType(),
					RenderMode::TRIANGLELIST,
				}
			);
		}
	}
}

