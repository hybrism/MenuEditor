#include "pch.h"

#include "TextureDatabase.h"

#include "ModelFactory.h"
#include "AnimationFactory.h"
#include "TextureFactory.h"
#include "BinaryReader/BinaryVertexPaintingFileFactory.h"
#include <engine/graphics/animation/AnimationController.h>
#include <engine/graphics/Texture.h>
#include "engine\graphics\PrimitiveMeshes\SpherePrimitive.h"
#include "engine\graphics\PrimitiveMeshes\CubePrimitive.h"
#include <engine/utility/StringHelper.h>
#include <engine/utility/Error.h>

#undef snprintf
#include <nlohmann/json.hpp>

TextureDatabase::TextureDatabase()
{
}

TextureDatabase::~TextureDatabase()
{
	myTextures.clear();
}

size_t TextureDatabase::GetTextureIndex(const std::string& aMaterialName)
{
	std::string findStr = StringHelper::ToLower(aMaterialName.c_str());
	assert(myMaterialNameToTextureIndex.find(findStr) != myMaterialNameToTextureIndex.end() && "Material name does not exist");
	return myMaterialNameToTextureIndex.at(findStr);
}

bool TextureDatabase::DoesTextureExist(const std::string& aMaterialName)
{
	std::string findStr = StringHelper::ToLower(aMaterialName.c_str());
	return myMaterialNameToTextureIndex.find(findStr) != myMaterialNameToTextureIndex.end();
}

std::shared_ptr<Texture> TextureDatabase::GetOrLoadSpriteTexture(const std::string& aTextureName)
{
	if (!myTextureNameToSpriteTextureIndex.contains(aTextureName))
	{
		mySpriteTextures.push_back({ aTextureName, TextureFactory::CreateTexture(RELATIVE_SPRITE_ASSET_PATH + aTextureName, false) });
		myTextureNameToSpriteTextureIndex[aTextureName] = mySpriteTextures.size() - 1;
	}

	return mySpriteTextures[myTextureNameToSpriteTextureIndex[aTextureName]].texture;
}

void TextureDatabase::ReadTextures(const nlohmann::json& jsonObject)
{
	const char* jsonKey = "textures";
	if (!jsonObject.contains(jsonKey)) { return; }

	std::unordered_map<size_t, size_t> unityTextureIdToMeshId;

	for (auto& json : jsonObject[jsonKey]) {
		size_t unityId = json["textureID"].get<size_t>();

		assert(unityTextureIdToMeshId.find(unityId) == unityTextureIdToMeshId.end() && "Trying to add texture that already exist");

		std::string materialName = json["albedoPath"].get<std::string>();
		size_t textureId = myTextures.size();

		// material name extraction
		{
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

			materialName = StringHelper::ToLower(materialName);
			myMaterialNameToTextureIndex.insert({ materialName, textureId });
		}

		unityTextureIdToMeshId.insert({ unityId, textureId });

		myTextures.push_back(
			TextureCollection {
				{ json["albedoPath"].get<std::string>(), TextureFactory::CreateTexture(json["albedoPath"].get<std::string>()) },
				{ json["normalPath"].get<std::string>(), TextureFactory::CreateTexture(json["normalPath"].get<std::string>()) },
				{ json["materialPath"].get<std::string>(), TextureFactory::CreateTexture(json["materialPath"].get<std::string>()) },
				{ json["fxPath"].get<std::string>(), TextureFactory::CreateTexture(json["fxPath"].get<std::string>()) },
			}
		);
	}

	unityTextureIdToMeshId.clear();
}

void TextureDatabase::LoadVertexTextures(const std::string& aSceneName)
{
	//size_t index = myMaterialNameToTextureIndex.at(aMaterialName);
	std::string folderPath = std::string(RELATIVE_VERTEX_TEXTURE_ASSET_PATH) + aSceneName + "/";
	//+aMaterialName + ".dds"
	//myTextures[index].vertexData = { TextureFactory::CreateTexture(vertexTexturePath, false, false), vertexTexturePath };
	
	namespace fs = std::filesystem;

	if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
		return;
	}

	myVertexTextures.clear();
	myVertexTextures = BinaryVertexPaintingFileFactory::LoadVertexDefinitionsFromFile(folderPath);

	myMeshNameToVertexTextureIndex.clear();
	for (size_t i = 0; i < myVertexTextures.size(); i++)
	{
		std::string meshName = myVertexTextures[i].meshName + "_" + std::to_string(myVertexTextures[i].subMeshIndex);
		myMeshNameToVertexTextureIndex.insert({ meshName, i });
	}
	//for (const auto& entry : fs::directory_iterator(folderPath)) {
	//	if (fs::is_regular_file(entry.path())) {
	//		std::string path = entry.path().string();
	//		std::string materialName = StringHelper::GetNameFromPath(path);

	//		VertexTextureCollection collection;

	//		collection.vertex.texture = TextureFactory::CreateTexture(path, false, false);
	//		collection.vertex.path = path;

	//		myVertexTextures.push_back(collection);
	//	}
	//}
}

#ifdef _EDITOR
// TODO: MOVE THIS TO VERTEX PAINTER CLASS
unsigned int TextureDatabase::CreateVertexTexture(const VertexTextureCollection& aCollection)
{
	for (size_t i = 0; i < myVertexTextures.size(); i++)
	{
		if (myVertexTextures[i].meshName == aCollection.meshName &&
			myVertexTextures[i].subMeshIndex == aCollection.subMeshIndex)
		{
			myVertexTextures[i] = aCollection;
			return static_cast<unsigned int>(i);
		}
	}
	myVertexTextures.push_back(aCollection);
	std::string meshName = myVertexTextures.back().meshName + "_" + std::to_string(myVertexTextures.back().subMeshIndex);
	myMeshNameToVertexTextureIndex.insert({ meshName, myVertexTextures.size() - 1 });
	return static_cast<unsigned int>(myVertexTextures.size() - 1);
}

// TODO: MOVE THIS TO VERTEX PAINTER CLASS
void TextureDatabase::ReplaceBinaryVertexPaintedTextureFile(const std::string& aSceneName, const VertexIndexCollection& aCollection)
{
	std::string folderPath = std::string(RELATIVE_VERTEX_TEXTURE_ASSET_PATH) + aSceneName + "/";

	// THIS IS A TEMPORARY SOLUTION UNTIL LOAD ON INIT IN VERTEX PAINTING IS IMPLEMENTED
	{
		std::filesystem::remove(folderPath + vertexTextureDefinitionsName);
	}

	std::vector<VertexTextureCollectionImport> data{};
	for (size_t i = 0; i < myVertexTextures.size(); i++)
	{
		auto& v = myVertexTextures[i];
		size_t index = data.size() - 1;
		
		if (v.subMeshIndex == 0)
		{
			data.push_back({});
			VertexTextureCollectionImport& imp = data.back();

			imp.meshName = v.meshName;
			imp.sceneName = aSceneName;

			imp.materials.push_back(v.materials);
		}
		else
		{
			data[index].materials.push_back(v.materials);
		}
	}
	BinaryVertexPaintingFileFactory::WriteCollectionToFile(data, folderPath);
	BinaryVertexPaintingFileFactory::WriteVertexIndicesToFile(aSceneName, aCollection);
}
#endif
