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
			VertexTextureCollection textureCollection;

			textureCollection.meshName = meshName;
			textureCollection.subMeshIndex = j;

			// vertex texture path
			{
				std::string scenePath = RELATIVE_VERTEX_TEXTURE_ASSET_PATH + sceneName + "/" + textureCollection.meshName + "_" + std::to_string(j) + ".dds";

				textureCollection.materials.vertex.path = scenePath;
				textureCollection.materials.vertex.texture = TextureFactory::CreateTexture(scenePath, false, false);
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

			data.push_back(textureCollection);
		}
	}

	file.close();

	return data;
}
