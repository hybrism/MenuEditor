#pragma once
#include <string>

struct Animation;
class AnimationPlayer;
class Texture;

struct AssetMetaData
{
	std::string path = "";
	size_t id = 0;
};

struct TextureData
{
	std::string path = "";
	Texture* texture = nullptr;
};

struct TextureCollection
{
	TextureData albedoTexture{};
	TextureData normalTexture{};
	TextureData materialTexture{};
	TextureData emissiveTexture{};
	int vertexPaintIndex = -1;
};

struct MaterialName
{
	std::string name = "";
	int id = -1;
};

struct VertexMaterialCollection
{
	TextureData vertex{};
	MaterialName r{};
	MaterialName g{};
	MaterialName b{};
	MaterialName a{};
};

struct VertexTextureCollection
{
	VertexMaterialCollection materials{};
	std::string meshName{};
	size_t subMeshIndex = 0; // used for debugging, can be removed
};
