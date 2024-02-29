#pragma once
#include <unordered_map>
#include <cassert>
#include <vector>

#include "AssetDefines.h"
#include "FactoryStructs.h"

#include <nlohmann\json_fwd.hpp>
#include <engine\graphics\Light\LightStructs.h>

class SharedMesh;
class TextureFactory;
class AnimationController;

enum class PrimitiveMeshID
{
	Sphere = 0,
	Cube
};

class AssetDatabase
{
public:
	inline static void CreateInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new AssetDatabase();
		}
	}

	inline static void DestroyInstance()
	{
		if (myInstance != nullptr)
		{
			delete myInstance;
			myInstance = nullptr;
		}
	}
	~AssetDatabase();

	static void LoadAssetsFromJson(const std::string& aJsonPath);

	//const std::string& GetAssetPath(const size_t& aId) const;
	static const SharedMeshPackage& GetMesh(const size_t& aId)
	{
		assert(myInstance->myMeshes.size() > aId && "ID does not exist");
		return myInstance->myMeshes.at(aId);
	}

	static const SharedMeshPackage& GetMesh(const std::string& aMeshName)
	{
		assert(myInstance->myMeshNameToMeshId.find(aMeshName) != myInstance->myMeshNameToMeshId.end() && "Mesh Name does not exist");
		size_t index = myInstance->myMeshNameToMeshId.at(aMeshName);
		return myInstance->myMeshes.at(index);
	}

	static std::string GetMeshPath(size_t aMeshPathID)
	{
		assert(myInstance->myMeshPaths.size() > aMeshPathID && "ID does not exist");
		return myInstance->myMeshPaths.at(aMeshPathID);
	}

	static size_t GetMeshIdFromUnityId(size_t aUnityMeshID)
	{
		assert(myInstance->myUnityMeshIdToMeshId.find(aUnityMeshID) != myInstance->myUnityMeshIdToMeshId.end() && "ID does not exist");
		return myInstance->myUnityMeshIdToMeshId.at(aUnityMeshID);
	}

	static size_t GetMeshIdFromName(std::string aMeshName)
	{
		assert(myInstance->myMeshNameToMeshId.find(aMeshName) != myInstance->myMeshNameToMeshId.end() && "ID does not exist");
		return myInstance->myMeshNameToMeshId.at(aMeshName);
	}

	static const TextureCollection& GetTextures(const size_t& aId)
	{
		assert(myInstance->myTextures.find(aId) != myInstance->myTextures.end() && "ID does not exist");
		return myInstance->myTextures.at(aId);
	}

	static std::vector<Animation*> GetAnimations(const size_t& aMeshId) 
	{
		return myInstance->myAnimations.at(aMeshId);
	}

	static Animation* GetAnimation(const size_t& aMeshId, const size_t& aAnimationIndex)
	{
		assert(myInstance->myAnimations.at(aMeshId).size() > aAnimationIndex && "Animation does not exist");
		return GetAnimations(aMeshId)[aAnimationIndex];
	}

	static Animation* GetAnimation(const size_t& aMeshId, const std::string& aAnimationName)
	{
		return myInstance->GetAnimation(aMeshId, myInstance->myAnimationNameToIndex.at(aAnimationName));
	}

	static size_t GetAnimationIndex(const std::string& aAnimationName)
	{
		assert(myInstance->myAnimationNameToIndex.find(aAnimationName) != myInstance->myAnimationNameToIndex.end() && "Animation name does not exist");
		return myInstance->myAnimationNameToIndex.at(aAnimationName);
	}

	static size_t GetTextureIndex(const std::string& aMaterialName)
	{
		assert(myInstance->myMaterialNameToTextureIndex.find(aMaterialName) != myInstance->myMaterialNameToTextureIndex.end() && "Material name does not exist");
		return myInstance->myMaterialNameToTextureIndex.at(aMaterialName);
	}

	static bool DoesTextureExist(const std::string& aMaterialName)
	{
		return myInstance->myMaterialNameToTextureIndex.find(aMaterialName) != myInstance->myMaterialNameToTextureIndex.end();
	}

	static bool DoesAnimationControllerExist(const size_t& aMeshId)
	{
		return myInstance->myAnimationControllers.find(aMeshId) != myInstance->myAnimationControllers.end();
	}

	static std::string GetNameFromPath(std::string aPath)
	{
		size_t n = aPath.rfind("/");
		if (n != std::string::npos)
		{
			aPath = aPath.substr(n + 1);
		}

		n = aPath.rfind("\\");
		if (n != std::string::npos)
		{
			aPath = aPath.substr(n + 1);
		}

		n = aPath.rfind(".");
		if (n != std::string::npos)
		{
			aPath = aPath.substr(0, n);
		}

		return aPath;
	}

	static AnimationController* GetAnimationController(const size_t& aMeshId)
	{
		assert(DoesAnimationControllerExist(aMeshId) && "Animation controller does not exist");
		return myInstance->myAnimationControllers.at(aMeshId);
	}

	static void CreateAnimationController(const size_t& aMeshId);

	// TODO: GET RID OF THESE AFTER WE HAVE MOVED FROM UNITY
	static std::unordered_map<size_t, std::vector<Animation*>> GetAnimationMap() { return myInstance->myAnimations; }

	static volatile bool HasLoadedAssets() { return myInstance->myHasLoadedAssets; }

	static size_t GetMeshCount() { return myInstance->myMeshes.size(); }

	//LIGHTS
	static void StoreDirectionalLight(DirectionalLight aDirectionalLight);
	static void StorePointLight(PointLight aPointLight);
	static DirectionalLight& GetDirectionalLight();
	static std::vector<PointLight>& GetPointLight();

private:
	AssetDatabase();

	nlohmann::json OpenJson(std::string aJsonPath);
	void ReadMeshes(const nlohmann::json& jsonObject);
	void ReadTextures(const nlohmann::json& jsonObject);
	void ReadAnimations(const nlohmann::json& jsonObject);

	std::vector<SharedMeshPackage> myMeshes; // when moving from unity, change this to a vector
	std::unordered_map<size_t, TextureCollection> myTextures;
	std::vector<std::string> myMeshPaths;
	std::unordered_map<std::string, size_t> myMeshNameToMeshId;
	std::unordered_map<size_t, size_t> myUnityMeshIdToMeshId;
	std::unordered_map<size_t, AnimationController*> myAnimationControllers;
	std::unordered_map<std::string, size_t> myAnimationNameToIndex; // this only exists for convinience atm since we use unity to export animations
	std::unordered_map<std::string, size_t> myMaterialNameToTextureIndex;

	std::unordered_map<size_t, std::vector<Animation*>> myAnimations;

	const std::string myMissingPathText = "";

	volatile bool myHasLoadedAssets = false;
	static AssetDatabase* myInstance;


	//LIGHT
	std::vector<int> test;
	std::vector<PointLight> myStoredPointLightInformation;
	DirectionalLight myStoredDirectionalLightInformation;
};
