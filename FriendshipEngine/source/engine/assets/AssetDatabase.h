#pragma once
#include <unordered_map>
#include <cassert>
#include <vector>

#include "AssetDefines.h"
#include "FactoryStructs.h"
#include "TextureDatabase.h"

#include <nlohmann\json_fwd.hpp>
#include <engine\graphics\Light\LightStructs.h>

class SharedMesh;
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

#ifdef _EDITOR
	static SharedMeshPackage& GetMeshRef(const size_t& aId)
	{
		assert(myInstance->myMeshes.size() > aId && "ID does not exist");
		return myInstance->myMeshes.at(aId);
	}

	static SharedMeshPackage& GetMeshRef(const std::string& aMeshName)
	{
		assert(myInstance->myMeshNameToMeshId.find(aMeshName) != myInstance->myMeshNameToMeshId.end() && "Mesh Name does not exist");
		size_t index = myInstance->myMeshNameToMeshId.at(aMeshName);
		return myInstance->myMeshes.at(index);
	}

	// WARNING: may cause memory leaks if not handled properly
	static void ModifyTextures(const size_t& aId, const TextureCollection& aTextureCollection)
	{
		myInstance->myTextureDatabase.ModifyTextures(aId, aTextureCollection);
	}
#endif

	static void UpdateMeshTexture(const size_t& aTextureId, const size_t& aMeshId, const size_t& aMeshDataIndex);

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
		return myInstance->myTextureDatabase.GetTextures(aId);
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
		return myInstance->myTextureDatabase.GetTextureIndex(aMaterialName);
	}

	static bool DoesTextureExist(const std::string& aMaterialName)
	{
		return myInstance->myTextureDatabase.DoesTextureExist(aMaterialName);
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

	static void LoadVertexTextures(const std::string& aSceneName);

	//LIGHTS
	static void StoreDirectionalLight(DirectionalLight aDirectionalLight);
	static void StorePointLight(PointLight aPointLight);
	static DirectionalLight& GetDirectionalLight();
	static std::vector<PointLight>& GetPointLight();

	static TextureDatabase& GetTextureDatabase() { return myInstance->myTextureDatabase; }

private:
	AssetDatabase();

	nlohmann::json OpenJson(std::string aJsonPath);
	void ReadMeshes(const nlohmann::json& jsonObject);
	void ReadAnimations(const nlohmann::json& jsonObject);

	static AssetDatabase* myInstance;

	DirectionalLight myStoredDirectionalLightInformation;
	TextureDatabase myTextureDatabase;

	std::unordered_map<std::string, size_t> myMeshNameToMeshId;
	std::unordered_map<size_t, size_t> myUnityMeshIdToMeshId;
	std::unordered_map<size_t, AnimationController*> myAnimationControllers;
	std::unordered_map<std::string, size_t> myAnimationNameToIndex; // this only exists for convinience atm since we use unity to export animations
	std::unordered_map<size_t, std::vector<Animation*>> myAnimations;
	std::vector<SharedMeshPackage> myMeshes; // when moving from unity, change this to a vector
	std::vector<std::string> myMeshPaths;

	//LIGHT
	std::vector<int> test;
	std::vector<PointLight> myStoredPointLightInformation;

	const std::string myMissingPathText = "";
	volatile bool myHasLoadedAssets = false;
};
