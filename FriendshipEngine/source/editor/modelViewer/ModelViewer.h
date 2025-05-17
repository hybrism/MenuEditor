#pragma once
#include <wrl/client.h>
#include <shared/postMaster/Observer.h>

#include <assets/FactoryStructs.h>
#include <assets/AssetDefines.h>
#include <engine/graphics/model/MeshDrawerStructs.h>
#include <array>

#include <imgui/imgui.h>

class LightManager;

using Microsoft::WRL::ComPtr;
struct ID3D11ShaderResourceView;

class ModelViewer : public FE::Observer
{
	enum class eMeshType
	{
		Floor,
		Display,
		Gizmo,
		Count
	};

	enum class eWindowType
	{
		MissingTexturePopup,
		ResetToDefaultValues,
		Count
	};

	enum class eIcon
	{
		Home,
		Light,
		Camera,
		Mesh,
		LoadedMeshes,
		Goose,
		Count
	};

	//TODO MODELVIEWER: This only works for Vector3f right now
	enum class eInitialValues
	{
		CameraRotation,
		CameraPosition,
		LightDirection,
		LightColor,
		MeshPosition,
		MeshRotation,
		Count
	};

	struct ModelViewerMesh
	{
		MeshInstanceRenderData instance;
		SharedMeshPackage mesh;
	};

public:
	ModelViewer();
	~ModelViewer();

	void Init(LightManager* aLightManager);
	void Update(float dt);
	void Render();

private:
	std::array<ModelViewerMesh, (int)eMeshType::Count> myMeshes;
	std::array<bool, (int)eWindowType::Count> myPopups;
	std::array<ComPtr<ID3D11ShaderResourceView>, (int)eIcon::Count> myIcons;
	std::array<Vector3f, (int)eInitialValues::Count> myInitialValues;

	std::vector<ModelViewerMesh> myLoadedMeshes;
	std::vector<std::string> myCubemapPaths;

	SharedMeshPackage CreateMeshAndGetTextures(const std::string& aFilePath);
	TextureCollection FindTextures(const std::string& aFilePath, const std::string& aModelMaterial);
	bool CaseSensitiveComparison(const std::string& aTextureFile, const std::string& aPath);

	void CreateTextures(const std::string& aFilePath, TextureCollection& aTextureCollection);
	bool CouldFindTextures(const TextureCollection& aTextureCollection);

	//TODO MODELVIEWER: REMOVE THIS WHEN DEBUG CAMERA IS STABLE
	void CameraMovement(float dt);
	void RotateCameraWithMouse(float dt);
	ImVec2 myPreviousMousePosition;
	ImVec2 myCurrentMousePosition;
	float myCameraMovementSpeed = 300.f;
	float myMoveMultiplier = 3.f;
	float myCameraYaw = 180;
	float myCameraPitch = 0;
	bool myActiveSpeedMultiplier = false;
	// ^^^

	LightManager* myLightManager = nullptr;

	void RecieveMessage(const FE::Message& aMessage) override;

	// ----- IMGUI WINDOWS
	void ModelWindow();
	void SettingsWindow();

	// SETTINGS SUB-CATERORIES
	void LightingSettings();
	void CameraSettings();
	void MeshAndTexturesData();
	void PreviousMeshes();
	void HandleWarningPopups();
	std::string myMissingTexturesData;

	void SetAllValuesToDefault();
	void SetMeshToDefault();
	void SetLightToDefault();
	void SetCameraToDefault();
};