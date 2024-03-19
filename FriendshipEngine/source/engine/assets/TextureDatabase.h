#pragma once
#include <unordered_map>
#include <cassert>
#include <vector>

#include "AssetDefines.h"

#include <nlohmann\json_fwd.hpp>

class TextureFactory;

class TextureDatabase
{
public:
	TextureDatabase();
	~TextureDatabase();

#ifdef _EDITOR
	// WARNING: may cause memory leaks if not handled properly
	void ModifyTextures(const size_t& aId, const TextureCollection& aTextureCollection)
	{
		myTextures[aId] = aTextureCollection;
	}

	VertexTextureCollection& GetVertexTextureRef(const size_t& aId)
	{
		assert(myVertexTextures.size() > aId && "ID does not exist");
		return myVertexTextures.at(aId);
	}

	unsigned int CreateVertexTexture(const VertexTextureCollection& aCollection);

	//void UpdateVertexPaintedTexture(const size_t& aTextureId, const size_t& aMeshId, const size_t& aMeshDataIndex);
	void UpdateVertexPaintedTextures(const std::string& aSceneName);

	std::vector<TextureCollection>& GetTexturesRef() { return myTextures; }
#endif


	const TextureCollection& GetTextures(const size_t& aId)
	{
		assert(myTextures.size() > aId && "ID does not exist");
		return myTextures.at(aId);
	}

	const VertexTextureCollection& GetVertexTextures(const size_t& aId)
	{
		assert(myVertexTextures.size() > aId && "ID does not exist");
		return myVertexTextures.at(aId);
	}

	size_t GetTextureIndex(const std::string& aMaterialName)
	{
		assert(myMaterialNameToTextureIndex.find(aMaterialName) != myMaterialNameToTextureIndex.end() && "Material name does not exist");
		return myMaterialNameToTextureIndex.at(aMaterialName);
	}

	bool DoesTextureExist(const std::string& aMaterialName)
	{
		return myMaterialNameToTextureIndex.find(aMaterialName) != myMaterialNameToTextureIndex.end();
	}

	int TryGetVertexTextureIndex(const std::string& aMeshName, const size_t aIndex)
	{
		std::string meshName = aMeshName + "_" + std::to_string(aIndex);
		if (myMeshNameToVertexTextureIndex.find(meshName) == myMeshNameToVertexTextureIndex.end()) { return -1; }

		return static_cast<int>(myMeshNameToVertexTextureIndex.at(meshName));
	}

	void ReadTextures(const nlohmann::json& jsonObject);
	void LoadVertexTextures(const std::string& aSceneName);
private:
	std::vector<TextureCollection> myTextures; // TODO: Change vertex color texture save locations to a separate vector that updates once for each scene
	std::vector<VertexTextureCollection> myVertexTextures; // TODO: Change vertex color texture save locations to a separate vector that updates once for each scene
	std::unordered_map<std::string, size_t> myMaterialNameToTextureIndex;
	std::unordered_map<std::string, size_t> myMeshNameToVertexTextureIndex;
};
