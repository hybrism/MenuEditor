#pragma once
#include <shared/postMaster/Observer.h>

#include <assets/FactoryStructs.h>
#include <assets/AssetDefines.h>
#include <engine/graphics/model/MeshDrawerStructs.h>
#include <assets/TextureFactory.h>
#include <array>

#include <imgui/imgui.h>

class ModelViewer : public FE::Observer
{
	enum class eMeshType
	{
		Floor,
		Display,
		Gizmo,
		Count
	};

	//TODO MODELVIEWER: Add more error messages here if errors occur
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

	struct Mesh
	{
		MeshInstanceRenderData instance;
		SharedMeshPackage mesh;
	};

public:
	ModelViewer();
	~ModelViewer();

	void Init();
	void Update(float dt);
	void Render();

private:

	std::array<Mesh, (int)eMeshType::Count> myMeshes;
	std::array<bool, (int)eWindowType::Count> myPopups;
	std::array<ComPtr<ID3D11ShaderResourceView>, (int)eIcon::Count> mySpriteTextures;
	std::array<Vector3f, (int)eInitialValues::Count> myInitialValues;

	std::vector<Mesh> myLoadedMeshes;
	std::vector<std::string> myCubemapPaths;

	TextureFactory myTextureFactory;
	SharedMeshPackage CreateMeshAndGetTextures(const std::string& aFilePath);
	TextureCollection FindTextures(const std::string& aFilePath, const std::string& aModelMaterial);
	void CreateTextures(const std::string& aFilePath, TextureCollection& aTextureCollection);
	bool CouldFindTextures(const TextureCollection& aTextureCollection);

	//TODO MODELVIEWER: REMOVE THIS WHEN CAMERA ROTATION WORKS
	void RotateCameraWithMouse(float dt);
	ImVec2 myPreviousMousePosition;
	ImVec2 myCurrentMousePosition;
	float myCameraPitch = 0;
	float myCameraYaw = 180;
	// ^^^

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