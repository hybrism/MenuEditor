#include "pch.h"

#include "AssetDatabase.h"

#include "ModelFactory.h"
#include "AnimationFactory.h"
#include "TextureFactory.h"
#include <engine/graphics/animation/AnimationController.h>
#include <engine/graphics/Texture.h>
#include "engine\graphics\PrimitiveMeshes\SpherePrimitive.h"

#undef snprintf
#include <nlohmann/json.hpp>

AssetDatabase* AssetDatabase::myInstance = nullptr;

AssetDatabase::AssetDatabase()
{
	myTextureFactory = new TextureFactory();



	SpherePrimitive sphere;
	sphere.ConstructSphere();

	SharedMeshPackage spherePackage;
	spherePackage.meshData.push_back(sphere.GetMesh());


	myMeshNameToMeshId.insert({ "PrimitiveSphere", (int)PrimitiveMeshID::Sphere });
	myUnityMeshIdToMeshId.insert({ (int)PrimitiveMeshID::Sphere,  (int)PrimitiveMeshID::Sphere });
	myMeshes.push_back(spherePackage);
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

	for (auto& pair : myTextures)
	{
		delete pair.second.albedoTexture.texture;
		delete pair.second.materialTexture.texture;
		delete pair.second.normalTexture.texture;
		delete pair.second.emissiveTexture.texture;
	}
	myTextures.clear();

	for (auto& pair : myAnimationControllers)
	{
		delete pair.second;
	}
	myAnimationControllers.clear();

	delete myTextureFactory;
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
	myInstance->ReadTextures(jsonRootObject);
	myInstance->ReadMeshes(jsonRootObject);
	myInstance->ReadAnimations(jsonRootObject);
	myInstance->myHasLoadedAssets = true;
}

#include <engine/utility/StringHelper.h>
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
			if (mesh->GetMaterialPath().length() == 0 && textureId != -1)
			{
				auto& textures = GetTextures(textureId);
				mesh->SetTextures(textures);
			}
		}
	}
}

void AssetDatabase::ReadTextures(const nlohmann::json& jsonObject)
{
	const char* jsonKey = "textures";
	if (!jsonObject.contains(jsonKey)) { return; }

	for (auto& json : jsonObject[jsonKey]) {
		size_t id = json["textureID"].get<size_t>();

		assert(myTextures.find(id) == myTextures.end() && "Trying to add texture that already exist");

		// material name extraction
		{
			std::string materialName = json["albedoPath"].get<std::string>();

			size_t n = materialName.find("t_");
			if (n != std::string::npos)
			{
				materialName.erase(materialName.begin(), materialName.begin() + n + 2);
			}
			n = materialName.rfind("_");
			if (n != std::string::npos)
			{
				materialName.erase(materialName.begin() + n, materialName.end());
			}

			myMaterialNameToTextureIndex.insert({ materialName, id });
		}

		myTextures.insert({
			id,
			{
				{ myTextureFactory->CreateTexture(json["albedoPath"].get<std::string>()), json["albedoPath"].get<std::string>() },
				{ myTextureFactory->CreateTexture(json["normalPath"].get<std::string>()), json["normalPath"].get<std::string>() },
				{ myTextureFactory->CreateTexture(json["materialPath"].get<std::string>()), json["materialPath"].get<std::string>() },
				{ myTextureFactory->CreateTexture(json["fxPath"].get<std::string>()), json["fxPath"].get<std::string>() }
			}
			});
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

#ifdef _DEBUG
	for (auto& pair : myAnimations)
	{
		if (pair.second.size() == 0)
		{
			PrintE("Skeletal mesh is missing animations after import: " + myMeshPaths.at(pair.first));
		}
	}
#endif
}
