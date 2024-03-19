#include "ModelViewer.h"

// External
#include <filesystem>

// Engine
#include <engine/Paths.h>
#include <engine/Engine.h>
#include <engine/utility/StringHelper.h>
#include <engine/utility/Error.h>
#include <engine/graphics/Camera.h>
#include <engine/graphics/GraphicsEngine.h>
#include <engine/graphics/model/Mesh.h>
#include <engine/graphics/model/SkeletalMesh.h>
#include <engine/graphics/model/MeshDrawer.h>
#include <engine/graphics/Light/LightManager.h>
#include <engine/graphics/renderer/DeferredRenderer.h>

//Assets
#include <assets/ModelFactory.h>
#include <assets/FactoryStructs.h>
#include <assets/ShaderDatabase.h>
#include <engine/graphics/Texture.h>
#include <assets/TextureFactory.h>

//Internal
#include <shared/postMaster/PostMaster.h>

//AFTER BUILD:
// [ ] Copy bin + content to new folder, remove uneccesary files (unused .pdb and .exes for example)
// [ ] Test open .exe to see if it works
// [ ] Zip it and upload to stream! :)

ModelViewer::ModelViewer()
{
	myInitialValues[(int)eInitialValues::CameraPosition] = { 0, 250.f, 500.f };
	myInitialValues[(int)eInitialValues::CameraRotation] = { 0, 180, 0 };
	myInitialValues[(int)eInitialValues::LightDirection] = { 40.f, -50.f, 0.f };
	myInitialValues[(int)eInitialValues::LightColor] = { 1.f, 1.f, 1.f };
	myInitialValues[(int)eInitialValues::MeshPosition] = { 0, 0, 0 };
	myInitialValues[(int)eInitialValues::MeshRotation] = { 0, 0, 0 };

	for (size_t i = 0; i < (int)eWindowType::Count; i++)
	{
		myPopups[i] = false;
	}
}

ModelViewer::~ModelViewer() {}

void ModelViewer::Init(LightManager* aLightManager)
{
	myLightManager = aLightManager;

	//SUBSCRIBE TO EVENTS
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::MeshDropped);

	//SET VALUES TO LIGHT AND CAMERA
	auto graphicsEngine = GraphicsEngine::GetInstance();

	graphicsEngine->GetCamera()->GetTransform().SetPosition(myInitialValues[(int)eInitialValues::CameraPosition]);
	graphicsEngine->GetCamera()->GetTransform().SetEulerAngles(myInitialValues[(int)eInitialValues::CameraRotation]);
	myLightManager->SetDirectionalLightsColor({ myInitialValues[(int)eInitialValues::LightColor].x, myInitialValues[(int)eInitialValues::LightColor].y, myInitialValues[(int)eInitialValues::LightColor].z, 1.f });
	myLightManager->GetDirectionalLight().myDirectionalLightCamera->GetTransform().SetEulerAngles(myInitialValues[(int)eInitialValues::LightDirection]);

	//LOAD ICONS
	std::wstring assetPath = StringHelper::s2ws(RELATIVE_MODELVIEWER_ASSET_PATH);
	TextureFactory::CreateDDSTexture(assetPath + L"icons/s_home.dds", myIcons[(int)eIcon::Home]);
	TextureFactory::CreateDDSTexture(assetPath + L"icons/s_light.dds", myIcons[(int)eIcon::Light]);
	TextureFactory::CreateDDSTexture(assetPath + L"icons/s_camera.dds", myIcons[(int)eIcon::Camera]);
	TextureFactory::CreateDDSTexture(assetPath + L"icons/s_mesh.dds", myIcons[(int)eIcon::Mesh]);
	TextureFactory::CreateDDSTexture(assetPath + L"icons/s_file.dds", myIcons[(int)eIcon::LoadedMeshes]);
	TextureFactory::CreateDDSTexture(assetPath + L"icons/s_goose.dds", myIcons[(int)eIcon::Goose]);

	//GET CUBEMAP-PATHS
	for (const auto& entry : std::filesystem::directory_iterator(RELATIVE_CUBEMAP_ASSET_PATH))
	{
		auto ext = entry.path().extension();
		if (ext == ".dds")
		{
			myCubemapPaths.push_back(entry.path().filename().string());
		}
	}

	graphicsEngine->SetCubemap(std::string(RELATIVE_CUBEMAP_ASSET_PATH) + myCubemapPaths[10]);

	//INIT FLOOR & GIZMO
	myMeshes[(int)eMeshType::Floor].mesh = CreateMeshAndGetTextures(RELATIVE_MODELVIEWER_ASSET_PATH + (std::string)"models/sm_floor.fbx");
	myMeshes[(int)eMeshType::Floor].instance.vsType = VsType::DefaultPBRInstanced;
	myMeshes[(int)eMeshType::Floor].instance.psType = PsType::GBuffer;
	myMeshes[(int)eMeshType::Floor].instance.data = StaticMeshInstanceData();
	myMeshes[(int)eMeshType::Gizmo].mesh = CreateMeshAndGetTextures(RELATIVE_MODELVIEWER_ASSET_PATH + (std::string)"models/sm_gizmo.fbx");
	myMeshes[(int)eMeshType::Gizmo].instance.vsType = VsType::DefaultPBRInstanced;
	myMeshes[(int)eMeshType::Gizmo].instance.psType = PsType::GBuffer;
	myMeshes[(int)eMeshType::Gizmo].instance.data = myMeshes[(int)eMeshType::Floor].instance.data;

	myMeshes[(int)eMeshType::Display].instance.data = StaticMeshInstanceData();
}

void ModelViewer::Update(float dt)
{
	RotateCameraWithMouse(dt);
	CameraUpdate(dt);

	ModelWindow();
}



void ModelViewer::Render()
{
	GraphicsEngine* ge = GraphicsEngine::GetInstance();
	RenderState state;
	state.blendState = BlendState::Disabled;
	state.depthStencilState = DepthStencilState::ReadWrite;
	state.rasterizerState = RasterizerState::BackfaceCulling;
	ge->SetRenderState(state);

	for (size_t i = 0; i < (int)eMeshType::Count; i++)
	{
		if (i == (int)eMeshType::Gizmo) //Turn of Depth
			ge->SetRawBackBufferAsRenderTarget();
		else
			ge->SetRawBackBufferAsRenderTarget(&ge->GetDepthBuffer());

		for (size_t j = 0; j < myMeshes[i].mesh.meshData.size(); j++)
		{
			if (myMeshes[i].mesh.skeleton)
			{
				auto skeletalMesh = static_cast<SkeletalMesh*>(myMeshes[i].mesh.meshData[j]);
				skeletalMesh->Render(
					std::get<StaticMeshInstanceData>(myMeshes[i].instance.data).transform.GetMatrix(),
					skeletalMesh->GetVertexShader(),
					skeletalMesh->GetPixelShader()
				);
			}
			else
			{
				ge->GetDeferredRenderer().Render(
					(Mesh*)myMeshes[i].mesh.meshData[j],
					myMeshes[i].instance);
			}
		}
	}

	SettingsWindow();
}

void ModelViewer::RecieveMessage(const FE::Message& aMessage)
{
	switch (aMessage.myEventType)
	{
	case FE::eMessageType::MeshDropped:
	{
		std::string filename = std::any_cast<std::string>(aMessage.myMessage);
		PrintI(filename);

		myMeshes[(int)eMeshType::Display].mesh = CreateMeshAndGetTextures(filename);
		myMeshes[(int)eMeshType::Display].instance.psType = PsType::GBuffer;
		myMeshes[(int)eMeshType::Display].instance.vsType = VsType::DefaultPBRInstanced;
		myMeshes[(int)eMeshType::Display].instance.data = StaticMeshInstanceData();
		myMeshes[(int)eMeshType::Gizmo].instance.data = myMeshes[(int)eMeshType::Display].instance.data;

		myLoadedMeshes.push_back(myMeshes[(int)eMeshType::Display]);
		break;
	}
	default:
		break;
	}
}

void ModelViewer::ModelWindow()
{
	ImGuiWindowClass windowClass;
	windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
	ImGui::SetNextWindowClass(&windowClass);
	ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
	if (ImGui::Begin("ModelViewer", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
	{
		ImVec2 region = ImGui::GetContentRegionAvail();
		ImVec2 screenCursorPos = ImGui::GetCursorScreenPos();
		Vector2i dimensions = GraphicsEngine::GetInstance()->DX().GetViewportDimensions();

		float aspectRatio = (float)dimensions.x / (float)dimensions.y;
		if (region.x / region.y > aspectRatio)
		{
			region.x = region.y * aspectRatio;
		}
		else
		{
			region.y = region.x / aspectRatio;
		}

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImVec2(vMin.x + region.x, vMin.y + region.y);
		ImVec2 wPos = ImGui::GetWindowPos();

		ImVec2 vMinWin = wPos + vMin;
		ImVec2 vMaxWin = wPos + vMax;

		ImDrawList* tDrawList = ImGui::GetWindowDrawList();
		tDrawList->AddImage(GraphicsEngine::GetInstance()->GetBackBufferSRV().Get(), vMinWin, vMaxWin);

		HandleWarningPopups();
	}
	ImGui::End();
}

void ModelViewer::SettingsWindow()
{
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos, ImGuiCond_Appearing);
	if (ImGui::Begin("ModelViewer", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
	{
		if (ImGui::ImageButton("Mesh", myIcons[(int)eIcon::Mesh].Get(), ImVec2(64, 64)))
			ImGui::OpenPopup("Mesh and Textures");
		ImGui::SetItemTooltip("Edit mesh position and view textures");

		if (ImGui::ImageButton("Light", myIcons[(int)eIcon::Light].Get(), ImVec2(64, 64)))
			ImGui::OpenPopup("LightSettings");
		ImGui::SetItemTooltip("Edit directional light and cubemap");

		if (ImGui::ImageButton("Camera", myIcons[(int)eIcon::Camera].Get(), ImVec2(64, 64)))
			ImGui::OpenPopup("CameraSettings");
		ImGui::SetItemTooltip("Edit camera transform");

		if (ImGui::ImageButton("Home", myIcons[(int)eIcon::Home].Get(), ImVec2(64, 64)))
			myPopups[(int)eWindowType::ResetToDefaultValues] = true;
		ImGui::SetItemTooltip("Reset values to default");

		if (ImGui::ImageButton("LoadedMeshes", myIcons[(int)eIcon::LoadedMeshes].Get(), ImVec2(64, 64)))
			ImGui::OpenPopup("LoadedMeshes");
		ImGui::SetItemTooltip("View and load previous Meshes");

		if (ImGui::ImageButton("Goose", myIcons[(int)eIcon::Goose].Get(), ImVec2(64, 64)))
			ImGui::OpenPopup("LoadedMeshes");
		ImGui::SetItemTooltip("Secret button, no function here yet :p");

		CameraSettings();
		LightingSettings();
		MeshAndTexturesData();
		PreviousMeshes();
	}
	ImGui::End();
}

void ModelViewer::LightingSettings()
{
	if (ImGui::BeginPopup("LightSettings"))
	{
		auto graphicsEngine = GraphicsEngine::GetInstance();

		ImGui::SeparatorText("Lighting"); ImGui::Spacing();
		static int item_current_idx = 10;
		std::string combo_preview_value = myCubemapPaths[item_current_idx];
		if (ImGui::BeginCombo("Cubemaps", combo_preview_value.c_str()))
		{
			for (int n = 0; n < myCubemapPaths.size(); n++)
			{
				const bool is_selected = (item_current_idx == n);

				if (ImGui::Selectable(myCubemapPaths[n].c_str(), is_selected))
				{
					item_current_idx = n;
					graphicsEngine->SetCubemap(std::string(RELATIVE_CUBEMAP_ASSET_PATH) + myCubemapPaths[n]);
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::Spacing();


		if (ImGui::DragFloat2("Rotation", &myLightManager->GetDirectionalLight().myEuler.x))
		{
			myLightManager->GetDirectionalLight().myDirectionalLightCamera->GetTransform().SetEulerAngles(myLightManager->GetDirectionalLight().myEuler);
		}

		ImGui::DragFloat("Directional Intensity", &myLightManager->GetDirectionalLight().myIntensity, 0.05f, 0.0f);
		ImGui::DragFloat("Ambient Intensity", &myLightManager->GetDirectionalLight().myAmbientIntensity, 0.05f, 0.0f);


		ImGui::Spacing();
		if (ImGui::Button("Set Light to Default settings", ImVec2(ImGui::GetContentRegionAvail().x, 18)))
			SetLightToDefault();

		ImGui::EndPopup();
	}
}

void ModelViewer::CameraSettings()
{
	if (ImGui::BeginPopup("CameraSettings"))
	{
		auto graphicsEngine = GraphicsEngine::GetInstance();
		auto camera = graphicsEngine->GetCamera();

		Vector3f cameraPosition = camera->GetTransform().GetPosition();
		Vector3f cameraRotation = camera->GetTransform().GetEulerRotation();


		ImGui::SeparatorText("Camera"); ImGui::Spacing();

		if (ImGui::DragFloat3("Camera position", &cameraPosition.x))
			camera->GetTransform().SetPosition(cameraPosition);
		ImGui::Spacing();

		if (ImGui::DragFloat3("Camera rotation", &cameraRotation.x))
			camera->GetTransform().SetEulerAngles(cameraRotation);
		ImGui::Spacing();

		if (ImGui::Button("Set Camera to Default position", ImVec2(ImGui::GetContentRegionAvail().x, 18)))
			SetCameraToDefault();

		ImGui::EndPopup();
	}
}

void ModelViewer::MeshAndTexturesData()
{
	if (myLoadedMeshes.empty())
	{
		if (ImGui::BeginPopup("Mesh and Textures"))
		{
			ImGui::SeparatorText("No Meshes loaded yet! :)");
			ImGui::Text("Drag and drop a .fbx to window to start");
			ImGui::EndPopup();
		}
		return;
	}

	if (ImGui::BeginPopup("Mesh and Textures"))
	{
		StaticMeshInstanceData& instanceData = std::get<StaticMeshInstanceData>(myMeshes[(int)eMeshType::Display].instance.data);
		Vector3f position = instanceData.transform.GetPosition();

		//MESH
		ImGui::SeparatorText("Mesh"); ImGui::Spacing();
		if (ImGui::DragFloat3("Mesh position", &position.x))
		{
			instanceData.transform.SetPosition(position);
			myMeshes[(int)eMeshType::Gizmo].instance.transform = myMeshes[(int)eMeshType::Display].instance.transform;
		}

		if (ImGui::Button("Set Mesh to Default position", ImVec2(ImGui::GetContentRegionAvail().x, 18)))
			SetMeshToDefault();

		ImGui::Spacing();

		//TEXTURE
		ImGui::SeparatorText("Textures"); ImGui::Spacing();
		if (ImGui::BeginTable("Textures", 2))
		{
			for (size_t i = 0; i < myMeshes[(int)eMeshType::Display].mesh.meshData.size(); i++)
			{
				TextureCollection textures = myMeshes[(int)eMeshType::Display].mesh.meshData[i]->GetTextures();
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Albedo:");
				if (textures.albedoTexture.texture != nullptr)
					ImGui::Image(textures.albedoTexture.texture->GetShaderResourceView(), ImVec2(128, 128));

				ImGui::TableSetColumnIndex(1);
				ImGui::Text("Material:");
				if (textures.materialTexture.texture != nullptr)
					ImGui::Image(textures.materialTexture.texture->GetShaderResourceView(), ImVec2(128, 128));

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("Normal:");
				if (textures.normalTexture.texture != nullptr)
					ImGui::Image(textures.normalTexture.texture->GetShaderResourceView(), ImVec2(128, 128));

				ImGui::TableSetColumnIndex(1);
				ImGui::Text("Emissive:");
				if (textures.emissiveTexture.texture != nullptr)
					ImGui::Image(textures.emissiveTexture.texture->GetShaderResourceView(), ImVec2(128, 128));
			}
			ImGui::EndTable();
		}
		ImGui::EndPopup();
	}
}

void ModelViewer::PreviousMeshes()
{
	if (myLoadedMeshes.empty())
	{
		if (ImGui::BeginPopup("LoadedMeshes"))
		{
			ImGui::SeparatorText("No Meshes loaded yet! :)");
			ImGui::EndPopup();
		}
		return;
	}

	if (ImGui::BeginPopup("LoadedMeshes"))
	{
		ImGui::SeparatorText("Loaded Meshes"); ImGui::Spacing();

		static int selectedMeshIndex = 0;
		if (ImGui::BeginChild("MeshesToPickFrom", ImVec2(128, 128)))
		{
			for (int i = 0; i < myLoadedMeshes.size(); i++)
			{
				const bool is_selected = (selectedMeshIndex == i);

				if (ImGui::Selectable(myLoadedMeshes[i].mesh.name.c_str(), is_selected))
				{
					selectedMeshIndex = i;
					myMeshes[(int)eMeshType::Display] = myLoadedMeshes[i];
				}
			}
			ImGui::EndChild();
		}

		ImGui::EndPopup();
	}
}

void ModelViewer::HandleWarningPopups()
{
	if (myPopups[(int)eWindowType::MissingTexturePopup])
		ImGui::OpenPopup("Missing Texture!");

	if (myPopups[(int)eWindowType::ResetToDefaultValues])
		ImGui::OpenPopup("Default Values!");

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Missing Texture!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Could not find all textures!\n\n%s", myMissingTexturesData.c_str());
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			myPopups[(int)eWindowType::MissingTexturePopup] = false;
			myMissingTexturesData = "";
		}

		ImGui::EndPopup();
	}

	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Default Values!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("This will reset light, camera and mesh values to default");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			SetAllValuesToDefault();
			ImGui::CloseCurrentPopup();
			myPopups[(int)eWindowType::ResetToDefaultValues] = false;
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();

		}

		ImGui::EndPopup();
	}
}

void ModelViewer::SetAllValuesToDefault()
{
	SetMeshToDefault();
	SetLightToDefault();
	SetCameraToDefault();
}

void ModelViewer::SetMeshToDefault()
{
	StaticMeshInstanceData& instanceData = std::get<StaticMeshInstanceData>(myMeshes[(int)eMeshType::Display].instance.data);
	instanceData.transform.SetPosition({ 0.f, 0.f, 0.f });

	myMeshes[(int)eMeshType::Gizmo].instance.transform = myMeshes[(int)eMeshType::Display].instance.transform;
}

void ModelViewer::SetLightToDefault()
{
	myLightManager->SetDirectionalLightsColor({ myInitialValues[(int)eInitialValues::LightColor].x, myInitialValues[(int)eInitialValues::LightColor].y, myInitialValues[(int)eInitialValues::LightColor].z, 1.f });
	myLightManager->GetDirectionalLight().myDirectionalLightCamera->GetTransform().SetEulerAngles(myInitialValues[(int)eInitialValues::LightDirection]);
}

void ModelViewer::SetCameraToDefault()
{
	auto graphicsEngine = GraphicsEngine::GetInstance();
	auto camera = graphicsEngine->GetCamera();
	camera->GetTransform().SetPosition(myInitialValues[(int)eInitialValues::CameraPosition]);
	camera->GetTransform().SetEulerAngles(myInitialValues[(int)eInitialValues::CameraRotation]);
	myCameraPitch = 0;
	myCameraYaw = 0;
}

SharedMeshPackage ModelViewer::CreateMeshAndGetTextures(const std::string& aFilePath)
{
	MeshDataPackage meshInformation = ModelFactory::LoadMeshFromFBX(aFilePath);

	SharedMeshPackage meshes = ModelFactory::GetSharedMeshFromPackage(meshInformation, false);

	for (size_t i = 0; i < meshes.meshData.size(); i++)
	{
		meshes.meshData[i]->SetMaterialName(meshInformation.meshData[i].materialName);

		TextureCollection textures = FindTextures(aFilePath, meshes.meshData[i]->GetMaterialName());

		meshes.meshData[i]->SetVertexShader(ShaderDatabase::GetVertexShader(VsType::DefaultPBRInstanced));
		meshes.meshData[i]->SetPixelShader(ShaderDatabase::GetPixelShader(PsType::MissingTextureLegacy));

		if (CouldFindTextures(textures))
		{
			meshes.meshData[i]->SetTextures(textures);
			meshes.meshData[i]->SetPixelShader(ShaderDatabase::GetPixelShader(PsType::GBuffer));
		}
		else
		{
			myPopups[(int)eWindowType::MissingTexturePopup] = true;
		}

		if (meshes.skeleton != nullptr)
		{
			meshes.meshData[i]->SetVertexShader(ShaderDatabase::GetVertexShader(VsType::SkinnedPBR));
		}
	}
	return meshes;
}

TextureCollection ModelViewer::FindTextures(const std::string& aFilePath, const std::string& aModelMaterial)
{
	myMissingTexturesData = "";

	std::string filename;
	std::string path;

	//EXTRACT PATH FROM FULL FILE-PATH
	size_t n = aFilePath.rfind("\\");
	if (n != std::string::npos)
	{
		filename = aFilePath.substr(n + 1);
		path = aFilePath.substr(0, n + 1);
	}

	n = aFilePath.rfind("/");
	if (n != std::string::npos)
	{
		filename = aFilePath.substr(n + 1);
		path = aFilePath.substr(0, n + 1);
	}

	// SEE IF WE CAN FIND TEXTURE FROM MODELMATERIAL
	std::string albedo = "t_" + aModelMaterial + "_c.dds";
	std::string material = "t_" + aModelMaterial + "_m.dds";
	std::string emissive = "t_" + aModelMaterial + "_fx.dds";
	std::string normal = "t_" + aModelMaterial + "_n.dds";

	TextureCollection textureCollection;

	if (CaseSensitiveComparison(albedo, path))
	{
		textureCollection.albedoTexture.path = albedo;
		textureCollection.materialTexture.path = material;
		textureCollection.normalTexture.path = normal;
		textureCollection.emissiveTexture.path = emissive;

		CreateTextures(path, textureCollection);
	}

	if (CouldFindTextures(textureCollection))
		return textureCollection;

	myMissingTexturesData += "Textures listed in fbx:\n";
	myMissingTexturesData += albedo + "\n";
	myMissingTexturesData += material + "\n";
	myMissingTexturesData += emissive + "\n";
	myMissingTexturesData += normal + "\n\n";

	//IF NOT, LOOK FOR TEXTURE BY FILENAME
	std::string materialName = "";

	//REMOVE .fbx
	size_t index = filename.find_last_of(".");
	if (index != std::string::npos)
	{
		materialName = filename.substr(0, index);
	}

	//REMOVE PREFIX (sm_ / sk_)
	index = materialName.find_first_of("_") + 1;
	if (index != std::string::npos)
	{
		materialName = materialName.substr(index);
	}

	albedo = "t_" + materialName + "_c.dds";
	material = "t_" + materialName + "_m.dds";
	emissive = "t_" + materialName + "_fx.dds";
	normal = "t_" + materialName + "_n.dds";

	if (CaseSensitiveComparison(albedo, path))
	{
		textureCollection.albedoTexture.path = albedo;
		textureCollection.materialTexture.path = material;
		textureCollection.normalTexture.path = normal;
		textureCollection.emissiveTexture.path = emissive;

		CreateTextures(path, textureCollection);
	}

	myMissingTexturesData += "Search by Filename:\n";
	myMissingTexturesData += albedo + "\n";
	myMissingTexturesData += material + "\n";
	myMissingTexturesData += emissive + "\n";
	myMissingTexturesData += normal + "\n\n";

	return textureCollection;
}

bool ModelViewer::CaseSensitiveComparison(const std::string& aTextureFile, const std::string& aPath)
{
	for (const auto& entry : std::filesystem::directory_iterator(aPath))
	{
		if (entry.path().filename().extension() != ".dds")
			continue;

		if (aTextureFile == entry.path().filename().string())
			return true;
	}

	return false;
}

void ModelViewer::CreateTextures(const std::string& aFilePath, TextureCollection& aTextureCollection)
{
	aTextureCollection.albedoTexture.texture = TextureFactory::CreateTexture(aFilePath + aTextureCollection.albedoTexture.path, false);
	aTextureCollection.materialTexture.texture = TextureFactory::CreateTexture(aFilePath + aTextureCollection.materialTexture.path, false);
	aTextureCollection.normalTexture.texture = TextureFactory::CreateTexture(aFilePath + aTextureCollection.normalTexture.path, false);
	aTextureCollection.emissiveTexture.texture = TextureFactory::CreateTexture(aFilePath + aTextureCollection.emissiveTexture.path, false);
}

bool ModelViewer::CouldFindTextures(const TextureCollection& aTextureCollection)
{
	return aTextureCollection.albedoTexture.texture != nullptr &&
		aTextureCollection.materialTexture.texture != nullptr &&
		aTextureCollection.normalTexture.texture != nullptr &&
		aTextureCollection.emissiveTexture.texture != nullptr;
}

void ModelViewer::RotateCameraWithMouse(float dt)
{
	auto graphicsEngine = GraphicsEngine::GetInstance();
	auto camera = graphicsEngine->GetCamera();
	myCurrentMousePosition = ImGui::GetMousePos();

	const float CAMERA_MOVE_X = 11.0f;
	const float CAMERA_MOVE_Y = 9.0f;

	if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
	{
		ImVec2 mouseMovement = myPreviousMousePosition - myCurrentMousePosition;

		float rotation_rate_x = CAMERA_MOVE_X * dt;
		float rotation_rate_y = -CAMERA_MOVE_Y * dt;

		myCameraYaw -= mouseMovement.x * rotation_rate_x;
		myCameraPitch += mouseMovement.y * rotation_rate_y;

		camera->GetTransform().SetEulerAngles({ myCameraPitch, myCameraYaw, 0 });
	}

	myPreviousMousePosition = myCurrentMousePosition;
}

void ModelViewer::CameraUpdate(float dt)
{
	auto graphicsEngine = GraphicsEngine::GetInstance();
	auto camera = graphicsEngine->GetCamera();

	myActiveSpeedMultiplier = ImGui::IsKeyDown(ImGuiKey_LeftShift);
	float multiplier = 1 + myMoveMultiplier * static_cast<int>(myActiveSpeedMultiplier);

	float movement = myCameraMovementSpeed * multiplier * dt;
	Vector3f direction = { 0.f, 0.f, 0.f };

	if (ImGui::IsKeyDown(ImGuiKey_W))
		direction += camera->GetTransform().GetForward();

	if (ImGui::IsKeyDown(ImGuiKey_S))
		direction -= camera->GetTransform().GetForward();

	if (ImGui::IsKeyDown(ImGuiKey_A))
		direction -= camera->GetTransform().GetRight();

	if (ImGui::IsKeyDown(ImGuiKey_D))
		direction += camera->GetTransform().GetRight();

	if (ImGui::IsKeyDown(ImGuiKey_E))
		direction += camera->GetTransform().GetUp();

	if (ImGui::IsKeyDown(ImGuiKey_Q))
		direction -= camera->GetTransform().GetUp();


	camera->GetTransform().Translate(direction * movement);

}