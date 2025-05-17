#pragma once
#include "../AssetDefines.h"
#include <vector>

class AssetDatabase;
class SharedMesh;
struct Skeleton;

struct VertexTextureCollectionImport
{
	std::string sceneName{};
	std::string meshName{};
	std::vector<VertexMaterialCollection> materials{};
};

struct VertexIndexCollection
{
	std::unordered_map<eid_t, unsigned int> vertexIndices{};
	std::vector<unsigned int> indexCounter{};
};

constexpr const char* vertexTextureDefinitionsName = "vertexDefines.data";
constexpr const char* vertexIndexDefinitionsName = "vertexIndexDefines.data";

class BinaryVertexPaintingFileFactory
{
public:

	/*	WRITE ORDER

		size size_t
		vertex painted meshes count
			sceneName		std::string
			meshName		std::string
			mesh sub count	size_t
				materialNameR	std::string
				materialNameG	std::string
				materialNameB	std::string
				materialNameA	std::string


		READ ORDER

		size size_t
		vertex painted meshes count
			sceneName		std::string
			meshName		std::string
			mesh sub count	size_t
				vertexTexturePath	std::string (RELATIVE_PATH/meshName_{index}.dds)
				materialNameR		std::string
				materialNameG		std::string
				materialNameB		std::string
				materialNameA		std::string
	*/
	static std::vector<VertexTextureCollection> LoadVertexDefinitionsFromFile(const std::string& aFolderPath);
	static void WriteCollectionToFile(std::vector<VertexTextureCollectionImport>& aData, std::string aFolderPath);


	/*	WRITE ORDER

		size size_t
			sceneName		unsigned int

		READ ORDER

		size size_t
			sceneName		unsigned int
	*/
	static VertexIndexCollection LoadVertexIndicesFromFile(const std::string& aSceneName);
	static void WriteVertexIndicesToFile(const std::string& aSceneName, const VertexIndexCollection& aCollection);
private:
	static std::string GetSceneName(const std::string& aFilePath);
};
