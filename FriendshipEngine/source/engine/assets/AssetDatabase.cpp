#include "pch.h"

#include "AssetDatabase.h"

#include "ModelFactory.h"
#include "AnimationFactory.h"
#include "TextureFactory.h"
#include <engine/graphics/animation/AnimationController.h>
#include <engine/graphics/Texture.h>
#include "engine\graphics\PrimitiveMeshes\SpherePrimitive.h"
#include "engine\graphics\PrimitiveMeshes\CubePrimitive.h"
#include <engine/utility/StringHelper.h>

#undef snprintf
#include <nlohmann/json.hpp>

AssetDatabase* AssetDatabase::myInstance = nullptr;



AssetDatabase::AssetDatabase()
{
	SpherePrimitive sphere;
	sphere.ConstructSphere();

	SharedMeshPackage spherePackage;
	spherePackage.meshData.push_back(sphere.GetMesh());


	myMeshNameToMeshId.insert({ "PrimitiveSphere", (int)PrimitiveMeshID::Sphere });
	myUnityMeshIdToMeshId.insert({ (int)PrimitiveMeshID::Sphere,  (int)PrimitiveMeshID::Sphere });
	myMeshes.push_back(spherePackage);
	myMeshPaths.push_back("NoPath");


	CubePrimitive cube;
	cube.ConstructCube();

	SharedMeshPackage cubePackage;
	cubePackage.meshData.push_back(cube.GetMesh());


	myMeshNameToMeshId.insert({ "PrimitiveCube", (int)PrimitiveMeshID::Cube });
	myUnityMeshIdToMeshId.insert({ (int)PrimitiveMeshID::Cube,  (int)PrimitiveMeshID::Cube });
	myMeshes.push_back(cubePackage);
	myMeshPaths.push_back("NoPath");
}

AssetDatabase::~AssetDatabase()
{
	for (auto& pair : myMeshes)
	{
		for (auto& mesh : pair.meshData)
		{
			delete mesh;
			mesh = nullptr;
		}

		if (pair.skeleton)
		{
			delete pair.skeleton;
			pair.skeleton = nullptr;
		}

		pair.name = "";
	}
	myMeshes.clear();

	for (auto& pair : myAnimationControllers)
	{
		delete pair.second;
	}
	myAnimationControllers.clear();
}

void AssetDatabase::CreateAnimationController(const size_t& aMeshId)
{
	assert(!DoesAnimationControllerExist(aMeshId) && "Animation controller already exists");
	myInstance->myAnimationControllers.insert({ aMeshId, new AnimationController(myInstance, aMeshId) });
}

nlohmann::json AssetDatabase::OpenJson(std::string aJsonPath)
{
	aJsonPath = RELATIVE_IMPORT_PATH + aJsonPath;
	assert(std::filesystem::exists(aJsonPath)/*, "Failed to find Json Path"*/);
	std::ifstream stream(aJsonPath);
	assert(stream.is_open()/*, "Failed to open stream"*/);

	std::string extension = aJsonPath.substr(aJsonPath.find_last_of(".") + 1);
	nlohmann::json jsonDirectory;

	if (extension == "json")
	{
		std::ifstream t(aJsonPath);
		assert(t.is_open());
		jsonDirectory = nlohmann::json::parse(t);
		t.close();
	}
	else if (extension == "friend")
	{
		std::ifstream t(aJsonPath, std::ios::binary);
		assert(t.is_open());
		jsonDirectory = nlohmann::json::from_bson(t);
		t.close();
	}
	else
	{
		PrintW("Extension not compatible!");
	}

	assert(jsonDirectory.is_null() == false/*, "Failed to open nlohmann::json object from std::ifstream."*/);
	return jsonDirectory;
}

void AssetDatabase::LoadAssetsFromJson(const std::string& aJsonPath)
{
	myInstance->myHasLoadedAssets = false;
	nlohmann::json jsonRootObject = myInstance->OpenJson(aJsonPath);
	myInstance->myTextureDatabase.ReadTextures(jsonRootObject);
	myInstance->ReadMeshes(jsonRootObject);
	myInstance->ReadAnimations(jsonRootObject);
	myInstance->myHasLoadedAssets = true;
}

void AssetDatabase::UpdateMeshTexture(const size_t& aTextureId, const size_t& aMeshId, const size_t& aMeshDataIndex)
{
	myInstance->myMeshes[aMeshId].meshData[aMeshDataIndex]->SetTextures(myInstance->myTextureDatabase.GetTextures(aTextureId));
}

// TODO: Initialize different shaders?
void AssetDatabase::ReadMeshes(const nlohmann::json& jsonObject)
{
	const char* jsonKey = "meshes";
	if (!jsonObject.contains(jsonKey)) { return; }

	for (auto& json : jsonObject[jsonKey]) {
		AssetMetaData metaData = {};
		metaData.id = json["meshID"].get<size_t>();
		metaData.path = json["assetPath"].get<std::string>();

		std::string fileName = StringHelper::GetFileName(metaData.path);

		// if the mesh is already loaded, skip it
		if (myMeshNameToMeshId.find(fileName) != myMeshNameToMeshId.end())
		{
			PrintW("Trying to add mesh (ID: " + std::to_string(metaData.id) + " path: " + metaData.path + ") that has already been added to AssetDatabase");
			continue;
		}

		//myAssetMetaData.push_back(metaData);

		SharedMeshPackage package = ModelFactory::LoadMesh(RELATIVE_ASSET_PATH + metaData.path, this);

		if (package.meshData.size() == 0)
		{
			PrintE("Failed to load mesh: " + metaData.path);
			continue;
		}

		size_t meshId = myMeshes.size();
		myMeshNameToMeshId.insert({ fileName, meshId });
		myUnityMeshIdToMeshId.insert({ metaData.id, meshId });
		myMeshes.push_back(package);
		myMeshPaths.push_back(metaData.path);

		if (package.skeleton != nullptr)
		{
			myAnimations.insert({ meshId, {} });
			CreateAnimationController(meshId);
		}

		int textureId = json["defaultTextureID"].get<int>();

		for (auto& mesh : package.meshData)
		{
			if (mesh->GetMaterialName().length() == 0 && textureId != -1)
			{
				auto& textures = GetTextures(textureId);
				mesh->SetTextures(textures);
			}
		}
	}
}

// TODO: Add bestie files for animations
void AssetDatabase::ReadAnimations(const nlohmann::json& jsonObject)
{
	const char* jsonKey = "animations";
	if (!jsonObject.contains(jsonKey)) { return; }

	for (auto& json : jsonObject[jsonKey]) {
		size_t meshId = json["meshID"].get<size_t>();
		std::string path = json["assetPath"].get<std::string>();
		std::string name = json["name"].get<std::string>();

		meshId = myUnityMeshIdToMeshId[meshId];

		Animation* animation = AnimationFactory::LoadAnimation(RELATIVE_ASSET_PATH + path, this);



		if (myAnimations.find(meshId) == myAnimations.end())
		{
			myAnimations.insert({ meshId, {} });
		}
		myAnimations.at(meshId).push_back(animation);
		size_t animationIndex = myAnimations[meshId].size() - 1;

		//myAnimationNameToIndex.insert({ name, animationIndex });
		myAnimationNameToIndex.insert({ animation->name, animationIndex });
	}

#ifndef _RELEASE
	for (auto& pair : myAnimations)
	{
		if (pair.second.size() == 0)
		{
			PrintE("Skeletal mesh is missing animations after import: " + myMeshPaths.at(pair.first));
		}
	}
#endif
}

void AssetDatabase::LoadVertexTextures(const std::string& aSceneName)
{
	myInstance->myTextureDatabase.LoadVertexTextures(aSceneName);

	for (size_t i = 0; i < myInstance->myMeshes.size(); i++)
	{
		auto& container = myInstance->myMeshes[i];

		for (size_t j = 0; j < container.meshData.size(); j++)
		{
			container.meshData[j]->SetVertexTextureId(
				myInstance->myTextureDatabase.TryGetVertexTextureIndex(container.meshData[j]->GetFileName(), j)
			);
		}
		//myInstance->myMeshes[i].meshData[0]->SetVertexTextureId(myInstance->myTextureDatabase.GetTextures(i));
	}
}

void AssetDatabase::StoreDirectionalLight(DirectionalLight aDirectionalLight)
{
	myInstance->myStoredDirectionalLightInformation = aDirectionalLight;
}

void AssetDatabase::StorePointLight(PointLight aPointLight)
{
	myInstance->myStoredPointLightInformation.push_back(aPointLight);
}

DirectionalLight& AssetDatabase::GetDirectionalLight()
{
	return myInstance->myStoredDirectionalLightInformation;
}

std::vector<PointLight>& AssetDatabase::GetPointLight()
{
	return myInstance->myStoredPointLightInformation;
}