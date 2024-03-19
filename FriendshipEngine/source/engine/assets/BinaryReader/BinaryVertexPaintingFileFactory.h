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

constexpr const char* vertexTextureDefinitionsName = "vertexDefines.data";

class BinaryVertexPaintingFileFactory
{
public:
	static std::vector<VertexTextureCollection> LoadVertexDefinitionsFromFile(const std::string& aFolderPath);
	static void WriteCollectionToFile(std::vector<VertexTextureCollectionImport>& aData, std::string aFolderPath);
private:
	static std::string GetSceneName(const std::string& aFilePath);
};
