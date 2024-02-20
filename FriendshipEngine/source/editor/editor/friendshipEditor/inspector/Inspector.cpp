#include "Inspector.h"

#include <ecs/World.h>

#include <assets/AssetDatabase.h>
#include <assets/AssetDefines.h>
#include <assets/FactoryStructs.h>

#include <engine/graphics/model/SharedMesh.h>
#include <engine/graphics/Texture.h>

#include <game/component/TransformComponent.h>
#include <game/component/MetaDataComponent.h>
#include <game/component/PlayerComponent.h>
#include <game/component/MeshComponent.h>
#include <game/component/EnemyComponent.h>
#include <game/component/CollisionDataComponent.h>
#include <game/component/ColliderComponent.h>
#include <game/component/CameraComponent.h>
#include <game/component/EventComponent.h>
#include <game/component/ScriptableEventComponent.h>
#include <game/component/OrbComponent.h>
#include <game/utility/Events.h>

FE::Inspector::Inspector(Entity aEntity, int aWindowID)
	: WindowBase("", true, ImGuiWindowFlags_None)
{
	myData.handle = "Inspector " + std::to_string(aWindowID);
	myWindowID = aWindowID;
	mySelectedEntity = aEntity;
	myTextureToDisplay = nullptr;
	myTextureWindowOpen = false;
}

void FE::Inspector::Show(const EditorUpdateContext& aContext)
{
	DisplayMetaData(aContext.world);
	DisplayTransformData(aContext.world);
	DisplayMeshAndTextureData(aContext.world);
	DisplayColliderData(aContext.world);
	DisplayPlayerData(aContext.world);
	DisplayEnemyData(aContext.world);
	DisplayScriptableEventData(aContext.world);
	DisplayOrbData(aContext.world);
}

void FE::Inspector::SetEntityID(const Entity& aEntity)
{
	mySelectedEntity = aEntity;
}

void FE::Inspector::Header(const std::string& aTitle)
{
	ImGui::Spacing(); ImGui::Spacing();
	ImGui::SeparatorText(aTitle.c_str());
	ImGui::Spacing();
}

void FE::Inspector::DisplayMetaData(World* aWorld)
{
	if (aWorld->TryGetComponent<MetaDataComponent>(mySelectedEntity))
	{
		auto& data = aWorld->GetComponent<MetaDataComponent>(mySelectedEntity);
		std::string id = std::to_string(mySelectedEntity);
		std::string unityID = std::to_string(data.unityID);

		Header("MetaComponent");
		ImGui::Text("ID: %s", id.c_str());
		ImGui::Text("Unity ID: %s", unityID.c_str());
		ImGui::Text("Name: %s", data.name);
	}
}

void FE::Inspector::DisplayTransformData(World* aWorld)
{
	if (aWorld->TryGetComponent<TransformComponent>(mySelectedEntity))
	{
		auto& t = aWorld->GetComponent<TransformComponent>(mySelectedEntity);
		auto& transform = t.transform;
		Vector3f pos = transform.GetPosition();
		Vector3f rot = transform.GetEulerRotation();
		Vector3f scale = transform.GetScale();

		Header("TransformComponent");
		ImGui::DragFloat3("Position", &pos.x);
		ImGui::DragFloat3("Rotation", &rot.x, 0.1f, 0.f, 360.f);
		ImGui::DragFloat3("Scale (Can't edit atm)", &scale.x, 0.1f, 0.f);

		std::string parent = t.parent == INVALID_ENTITY ? "N/A" : std::to_string(t.parent);
		ImGui::Text("Parent: %s", parent.c_str());

		if (ImGui::TreeNode("Children:"))
		{
			for (size_t i = 0; i < t.children.size(); i++)
			{
				if (t.children[i] == 0) { continue; }
				ImGui::Text(std::to_string(t.children[i]).c_str());
			}

			ImGui::TreePop();
		}
	}
}

void FE::Inspector::DisplayMeshAndTextureData(World* aWorld)
{
	if (aWorld->TryGetComponent<MeshComponent>(mySelectedEntity))
	{
		size_t assetID = aWorld->GetComponent<MeshComponent>(mySelectedEntity).id;

		Header("MeshComponent");

		if (ImGui::TreeNode("Model Path"))
		{
			std::string modelPath = AssetDatabase::GetMeshPath(assetID);
			ImGui::Text(modelPath.c_str());
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Textures"))
		{
			auto mesh = AssetDatabase::GetMesh(assetID);

			for (auto data : mesh.meshData)
			{
				auto textureCollection = data->GetTextures();

				if (textureCollection.albedoTexture.texture == nullptr)
				{
					ImGui::Text("No textures found!");
					continue;
				}

				if (ImGui::ImageButton(textureCollection.albedoTexture.texture->GetShaderResourceView(), ImVec2(64, 64)))
				{
					myTextureToDisplay = textureCollection.albedoTexture.texture;
					myTextureWindowOpen = true;
				}
				ImGui::SameLine();

				if (ImGui::ImageButton(textureCollection.normalTexture.texture->GetShaderResourceView(), ImVec2(64, 64)))
				{
					myTextureToDisplay = textureCollection.normalTexture.texture;
					myTextureWindowOpen = true;
				}
				ImGui::SameLine();

				if (ImGui::ImageButton(textureCollection.materialTexture.texture->GetShaderResourceView(), ImVec2(64, 64)))
				{
					myTextureToDisplay = textureCollection.materialTexture.texture;
					myTextureWindowOpen = true;
				}
				ImGui::SameLine();

				if (ImGui::ImageButton(textureCollection.emissiveTexture.texture->GetShaderResourceView(), ImVec2(64, 64)))
				{
					myTextureToDisplay = textureCollection.emissiveTexture.texture;
					myTextureWindowOpen = true;
				}
			}

			ImGui::TreePop();
		}

		if (myTextureToDisplay && myTextureWindowOpen)
		{
			ImGui::Begin("Texture", &myTextureWindowOpen);
			ImGui::Image(myTextureToDisplay->GetShaderResourceView(), ImVec2(512, 512));
			ImGui::End();
		}

		if (!myTextureToDisplay) { myTextureWindowOpen = false; }
	}
}

void FE::Inspector::DisplayColliderData(World* aWorld)
{
	if (aWorld->TryGetComponent<CollisionDataComponent>(mySelectedEntity) && aWorld->TryGetComponent<ColliderComponent>(mySelectedEntity))
	{
		auto& collisionData = aWorld->GetComponent<CollisionDataComponent>(mySelectedEntity);

		Header("CollisionDataComponent");

		std::string collisionStatus = collisionData.isColliding ? "Collision!" : "No Collision";
		ImGui::Text("Collision status: %s", collisionStatus.c_str());

		int collisionLayer = static_cast<int>(collisionData.layer);
		ImGui::Text("Layer:  %i", collisionLayer);

		auto& collider = aWorld->GetComponent<ColliderComponent>(mySelectedEntity);
		Vector3f extents = collider.extents;
		Vector3f aabbMin = collider.aabb3D.GetMin();
		Vector3f aabbMax = collider.aabb3D.GetMax();

		Header("ColliderComponent");
		if (ImGui::TreeNode("Size"))
		{
			ImGui::Text("AABB");
			ImGui::DragFloat3("Min", &aabbMin.x);
			ImGui::DragFloat3("Max", &aabbMax.x);
			ImGui::Spacing();
			ImGui::Text("Extents");
			ImGui::DragFloat3("Extents", &extents.x);
			ImGui::TreePop();
		}
	}
}

void FE::Inspector::DisplayPlayerData(World* aWorld)
{
	if (aWorld->TryGetComponent<PlayerComponent>(mySelectedEntity))
	{

		auto& p = aWorld->GetComponent<PlayerComponent>(mySelectedEntity);

		Header("PlayerComponent");

		ImGui::DragFloat2("Move Direction:", &p.inputDirection.x);
		ImGui::DragFloat2("Sensitivity:", &p.cameraSensitivity.x);
		ImGui::DragFloat3("Velocity:", &p.finalVelocity.x);
		ImGui::Checkbox("Is Grounded:", &p.isGrounded);
		ImGui::Checkbox("Is Wallrunning:", &p.isWallRunning);
		float currentSpeed = p.finalVelocity.Length();
		ImGui::DragFloat("Current speed:", &currentSpeed, 0, 0, 0, "%.3f", ImGuiSliderFlags_NoInput);

		ImGui::Checkbox("Is Crouching:", &p.isCrouching);
		ImGui::Checkbox("Is Sliding:", &p.isSliding);
	}
}

void FE::Inspector::DisplayEnemyData(World* aWorld)
{
	if (aWorld->TryGetComponent<EnemyComponent>(mySelectedEntity))
	{
		Header("EnemyComponent");
	}
}

void FE::Inspector::DisplayScriptableEventData(World* aWorld)
{
	if (aWorld->TryGetComponent<ScriptableEventComponent>(mySelectedEntity))
	{
		auto& sec = aWorld->GetComponent<ScriptableEventComponent>(mySelectedEntity);

		Header("ScriptableEventComponent");

		int entityToTrigger = static_cast<int>(sec.triggerEntityID);
		ImGui::DragInt("Trigger target ID", &entityToTrigger, ImGuiSliderFlags_NoInput);

		//int eventType = static_cast<int>(sec.eventToTrigger);
		//const char* typeNames[static_cast<int>(eEvent::Count)] = { "OpenDoor", "LevelUp", "Checkpoint", "Cutscene", "BossDead" };
		//const char* typeName = (eventType >= 0 && eventType < static_cast<int>(eEventType::Count)) ? typeNames[eventType] : "Unknown";
		//ImGui::SliderInt("Event Type", &eventType, 0, static_cast<int>(eEvent::Count) - 1, typeName, ImGuiSliderFlags_NoInput);
	}
}

void FE::Inspector::DisplayOrbData(World* aWorld)
{
	if (aWorld->TryGetComponent<OrbComponent>(mySelectedEntity))
	{
		OrbComponent& orbComponent = aWorld->GetComponent<OrbComponent>(mySelectedEntity);

		Header("OrbComponent");

		ImGui::DragFloat("ChaseSpeed", &orbComponent.ChaseSpeed);
	}
}