#pragma once
#include <unordered_map>
#include <cassert>
#include <vector>

#include "AssetDefines.h"
#include "FactoryStructs.h"
#include "TextureDatabase.h"

#include <nlohmann\json_fwd.hpp>
#include <engine\graphics\Light\LightStructs.h>
#include <engine/graphics/animation/AnimationController.h>
#include <engine/graphics/animation/Skeleton.h>

class SharedMesh;

enum class PrimitiveMeshID
{
	Cube = 0,
	Quad,
	Sphere,
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

	static std::vector<Animation*> GetAnimations(const Skeleton* aSkeleton) 
	{
		return myInstance->myAnimations.at(aSkeleton->name);
	}

	static Animation* GetAnimation(const Skeleton* aSkeleton, const size_t& aAnimationIndex)
	{
		assert(myInstance->myAnimations.at(aSkeleton->name).size() > aAnimationIndex && "Animation does not exist");
		return GetAnimations(aSkeleton)[aAnimationIndex];
	}

	static Animation* GetAnimation(const Skeleton* aSkeleton, const std::string& aAnimationName)
	{
		return myInstance->GetAnimation(aSkeleton, myInstance->myAnimationNameToIndex.at(aAnimationName));
	}

	static Skeleton* GetSkeleton(const size_t& aMeshId)
	{
		return myInstance->myMeshes[aMeshId].skeleton;
	}

	static Skeleton* GetSkeleton(const std::string& aMeshName)
	{
		return myInstance->myMeshes[GetMeshIdFromName(aMeshName)].skeleton;
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

	static bool DoesAnimationControllerExist(const Skeleton* aSkeleton)
	{
		return myInstance->mySkeletonToAnimationControllerId.find(aSkeleton->name) != myInstance->mySkeletonToAnimationControllerId.end();
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

	static AnimationController& GetAnimationController(const Skeleton* aSkeleton)
	{
		assert(DoesAnimationControllerExist(aSkeleton) && "Animation controller does not exist");
		return GetAnimationController(myInstance->mySkeletonToAnimationControllerId.at(aSkeleton->name));
	}

	static AnimationController& GetAnimationController(const size_t& aId)
	{
		assert(myInstance->myAnimationControllers.size() > aId && "ID does not exist");
		return myInstance->myAnimationControllers[aId];
	}

	static int CreateAnimationController(const Skeleton* aSkeleton);
	static int CreateAnimationController();

	// TODO: GET RID OF THESE AFTER WE HAVE MOVED FROM UNITY
	static std::unordered_map<std::string, std::vector<Animation*>> GetAnimationMap() { return myInstance->myAnimations; }

	static volatile bool HasLoadedAssets() { return myInstance->myHasLoadedAssets; }

	static size_t GetMeshCount() { return myInstance->myMeshes.size(); }

	static void LoadVertexTextures(const std::string& aSceneName);

	//LIGHTS
	static void StoreDirectionalLight(DirectionalLight aDirectionalLight);
	static void StorePointLight(PointLight aPointLight);
	static void ClearPointLight() 
	{ 
		myInstance->myStoredPointLightInformation.clear(); 
	}
	static DirectionalLight& GetDirectionalLight();
	static std::vector<PointLight> GetPointLight();

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
	std::unordered_map<std::string, int> mySkeletonToAnimationControllerId; // USE THIS MAP UNTIL WE HAVE MOVED FROM UNITY
	std::unordered_map<std::string, size_t> myAnimationNameToIndex; // this only exists for convinience atm since we use unity to export animations
	std::unordered_map<std::string, std::vector<Animation*>> myAnimations;
	std::vector<SharedMeshPackage> myMeshes; // when moving from unity, change this to a vector
	std::vector<std::string> myMeshPaths;
	std::vector<AnimationController> myAnimationControllers;

	//LIGHT
	std::vector<PointLight> myStoredPointLightInformation;

	const std::string myMissingPathText = "";
	volatile bool myHasLoadedAssets = false;
};
