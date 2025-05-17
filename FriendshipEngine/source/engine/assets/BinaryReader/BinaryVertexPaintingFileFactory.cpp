#include "pch.h"
#include "BinaryVertexPaintingFileFactory.h"
#include <filesystem>

#include <engine/Paths.h>
#include <engine/graphics/model/Mesh.h>
#include <engine/graphics/model/SkeletalMesh.h>
#include <engine/graphics/Vertex.h>
#include "engine/graphics/animation\Skeleton.h"
#include "engine/utility/Error.h"
#include "../TextureFactory.h"

#include "../AssetDatabase.h"
#include "../ShaderDatabase.h"
#include "BinaryFileUtility.h"
#include <ModelFactory.h>

VertexIndexCollection BinaryVertexPaintingFileFactory::LoadVertexIndicesFromFile(const std::string& aSceneName)
{
	std::string folderPath = std::string(RELATIVE_VERTEX_TEXTURE_ASSET_PATH) + aSceneName + "/";
	std::string path = folderPath + vertexIndexDefinitionsName;
	if (!std::filesystem::exists(path))
	{
		PrintW("Vertex texture definitions does not exist for current scene: " + path);
		return {};
	}

	std::ifstream file(path, std::ios::binary | std::ios::in);

	if (!file.is_open()) {
		PrintE("Error opening file for reading: " + path);
		return {};
	}

	VertexIndexCollection collection{};

	// vertex indices
	{
		size_t size = 0;
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));

		for (size_t i = 0; i < size; i++)
		{
			eid_t entity = 0;
			unsigned int index = 0;
			file.read(reinterpret_cast<char*>(&entity), sizeof(eid_t));
			file.read(reinterpret_cast<char*>(&index), sizeof(unsigned int));
			collection.vertexIndices[entity] = index;
		}
	}

	// index counter
	{
		size_t size = 0;
		file.read(reinterpret_cast<char*>(&size), sizeof(size_t));

		collection.indexCounter.resize(size, 0);

		for (size_t i = 0; i < size; i++)
		{
			BinaryFileUtility::ReadUintFromFile(file, collection.indexCounter[i]);
		}
	}

	file.close();

	return collection;
}

void BinaryVertexPaintingFileFactory::WriteVertexIndicesToFile(const std::string& aSceneName, const VertexIndexCollection& aCollection)
{
	std::string folderPath = std::string(RELATIVE_VERTEX_TEXTURE_ASSET_PATH) + aSceneName + "/";
	std::string path = folderPath + vertexIndexDefinitionsName;
	std::ofstream file(path, std::ios::binary | std::ios::out);

	if (!file.is_open()) {
		PrintE("Vertex Index Defines does not exist for scene. Generating file...");

		if (!std::filesystem::exists(folderPath)) {
			std::filesystem::create_directory(folderPath);
		}

		file.open(path, std::ios::binary | std::ios::out | std::ios::trunc);
		if (!file.is_open()) {
			PrintE("Error creating file: " + path);
			return;
		}
	}


	// vertex indices
	{
		size_t size = aCollection.vertexIndices.size();
		file.write(reinterpret_cast<const char*>(&size), sizeof(size_t));

		for (auto& pair : aCollection.vertexIndices)
		{
			file.write(reinterpret_cast<const char*>(&pair.first), sizeof(eid_t));
			file.write(reinterpret_cast<const char*>(&pair.second), sizeof(unsigned int));
		}
	}

	// index counter
	{
		size_t size = aCollection.indexCounter.size();
		file.write(reinterpret_cast<const char*>(&size), sizeof(size_t));

		for (size_t i = 0; i < size; i++)
		{
			BinaryFileUtility::WriteUintToFile(file, aCollection.indexCounter[i]);
		}
	}

	file.close();
}

std::string BinaryVertexPaintingFileFactory::GetSceneName(const std::string& aFilePath)
{
	std::string sceneName = aFilePath;
	sceneName = sceneName.substr(0, sceneName.find_last_of('/'));
	sceneName = sceneName.substr(sceneName.find_last_of('/') + 1);
	return sceneName;
}

void BinaryVertexPaintingFileFactory::WriteCollectionToFile(std::vector<VertexTextureCollectionImport>& aData, std::string aFolderPath)
{
	std::string path = aFolderPath + vertexTextureDefinitionsName;
	std::ofstream file(path, std::ios::binary | std::ios::out);

	if (!file.is_open()) {
		PrintE("Vertex Defines does not exist for scene. Generating file...");

		if (!std::filesystem::exists(aFolderPath)) {
			std::filesystem::create_directory(aFolderPath);
		}

		file.open(path, std::ios::binary | std::ios::out | std::ios::trunc);
		if (!file.is_open()) {
			PrintE("Error creating file: " + path);
			return;
		}
	}

	size_t dataSize = aData.size();
	file.write(reinterpret_cast<const char*>(&dataSize), sizeof(size_t));

	for (size_t i = 0; i < dataSize; i++)
	{
		// scene name
		BinaryFileUtility::WriteStringToFile(file, aData[i].sceneName);

		// mesh name
		BinaryFileUtility::WriteStringToFile(file, aData[i].meshName);

		// mesh sub count
		size_t size = aData[i].materials.size();
		BinaryFileUtility::WriteSizeTToFile(file, size);

		for (size_t j = 0; j < size; j++)
		{
			// materials
			BinaryFileUtility::WriteStringToFile(file, aData[i].materials[j].r.name);
			BinaryFileUtility::WriteStringToFile(file, aData[i].materials[j].g.name);
			BinaryFileUtility::WriteStringToFile(file, aData[i].materials[j].b.name);
			BinaryFileUtility::WriteStringToFile(file, aData[i].materials[j].a.name);
		}
	}

	file.close();
}

std::vector<VertexTextureCollection> BinaryVertexPaintingFileFactory::LoadVertexDefinitionsFromFile(const std::string& aFolderPath)
{
	std::string filePath = aFolderPath + vertexTextureDefinitionsName;
	if (!std::filesystem::exists(filePath))
	{
		PrintW("Vertex texture definitions does not exist for current scene: " + filePath);
		return {};
	}

	std::ifstream file(filePath, std::ios::binary | std::ios::in);

	if (!file.is_open()) {
		PrintE("Error opening file for reading: " + filePath);
		return {};
	}

	std::vector<VertexTextureCollection> data;

	size_t size = 0;
	file.read(reinterpret_cast<char*>(&size), sizeof(size_t));

	for (size_t i = 0; i < size; i++)
	{
		// scene name
		std::string sceneName{};
		BinaryFileUtility::ReadStringFromFile(file, sceneName); // scene name

		// mesh name
		std::string meshName{};
		BinaryFileUtility::ReadStringFromFile(file, meshName);

		// mesh sub count
		size_t count = 0;
		BinaryFileUtility::ReadSizeTFromFile(file, count);

		for (size_t j = 0; j < count; j++)
		{
			data.push_back({});

			VertexTextureCollection& textureCollection = data.back();

			textureCollection.meshName = meshName;
			textureCollection.subMeshIndex = j;

			// vertex texture path
			{
				std::string scenePath = RELATIVE_VERTEX_TEXTURE_ASSET_PATH + sceneName + "/" + textureCollection.meshName + "_" + std::to_string(j) + ".dds";

				textureCollection.materials.vertex.path = scenePath;
#ifdef _EDITOR
				textureCollection.materials.vertex.texture = TextureFactory::CreateDDSTextureWithCPUAccess(scenePath, textureCollection.materials.vertex.stagingTexture);
#else
				textureCollection.materials.vertex.texture = TextureFactory::CreateTexture(scenePath, false, false);
#endif
			}
			// materials
			{
				BinaryFileUtility::ReadStringFromFile(file, textureCollection.materials.r.name);
				textureCollection.materials.r.id = -1;
				if (AssetDatabase::DoesTextureExist(textureCollection.materials.r.name))
				{
					textureCollection.materials.r.id = static_cast<int>(AssetDatabase::GetTextureIndex(textureCollection.materials.r.name));
				}
			}
			{
				BinaryFileUtility::ReadStringFromFile(file, textureCollection.materials.g.name);
				textureCollection.materials.g.id = -1;
				if (AssetDatabase::DoesTextureExist(textureCollection.materials.g.name))
				{
					textureCollection.materials.g.id = static_cast<int>(AssetDatabase::GetTextureIndex(textureCollection.materials.g.name));
				}
			}
			{
				BinaryFileUtility::ReadStringFromFile(file, textureCollection.materials.b.name);
				textureCollection.materials.b.id = -1;
				if (AssetDatabase::DoesTextureExist(textureCollection.materials.b.name))
				{
					textureCollection.materials.b.id = static_cast<int>(AssetDatabase::GetTextureIndex(textureCollection.materials.b.name));
				}
			}
			{
				BinaryFileUtility::ReadStringFromFile(file, textureCollection.materials.a.name);
				textureCollection.materials.a.id = -1;
				if (AssetDatabase::DoesTextureExist(textureCollection.materials.a.name))
				{
					textureCollection.materials.a.id = static_cast<int>(AssetDatabase::GetTextureIndex(textureCollection.materials.a.name));
				}
			}

		}
	}

	file.close();

	return data;
}
