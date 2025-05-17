#pragma once
#include <engine/graphics/Texture.h>
#include <memory>

#ifdef _EDITOR
#include <engine/graphics/StagingTexture.h>
#endif

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
	std::shared_ptr<Texture> texture = nullptr;
#ifdef _EDITOR
	std::shared_ptr<StagingTexture> stagingTexture = nullptr;
#endif
};

struct TextureCollection
{
	TextureData albedoTexture{};
	TextureData normalTexture{};
	TextureData materialTexture{};
	TextureData emissiveTexture{};
	int vertexPaintIndex = -1;

	//TextureCollection() = default;

	//TextureCollection(TextureCollection&& aOther) noexcept
	//{
	//	albedoTexture = std::move(aOther.albedoTexture);
	//	normalTexture = std::move(aOther.normalTexture);
	//	materialTexture = std::move(aOther.materialTexture);
	//	emissiveTexture = std::move(aOther.emissiveTexture);
	//	vertexPaintIndex = aOther.vertexPaintIndex;
	//}

	//TextureCollection& operator=(TextureCollection&& aOther) noexcept
	//{
	//	albedoTexture = std::move(aOther.albedoTexture);
	//	normalTexture = std::move(aOther.normalTexture);
	//	materialTexture = std::move(aOther.materialTexture);
	//	emissiveTexture = std::move(aOther.emissiveTexture);
	//	vertexPaintIndex = aOther.vertexPaintIndex;
	//	return *this;
	//}

	//TextureCollection& operator=(const TextureCollection&& aOther) noexcept
	//{
	//	albedoTexture.path = aOther.albedoTexture.path;
	//	if (aOther.albedoTexture.texture != nullptr)
	//	{
	//		albedoTexture.texture = std::make_unique<Texture>(*aOther.albedoTexture.texture);
	//	}

	//	normalTexture.path = aOther.normalTexture.path;
	//	if (aOther.normalTexture.texture != nullptr)
	//	{
	//		normalTexture.texture = std::make_unique<Texture>(*aOther.normalTexture.texture);
	//	}

	//	materialTexture.path = aOther.materialTexture.path;
	//	if (aOther.materialTexture.texture != nullptr)
	//	{
	//		materialTexture.texture = std::make_unique<Texture>(*aOther.materialTexture.texture);
	//	}

	//	emissiveTexture.path = aOther.emissiveTexture.path;
	//	if (aOther.emissiveTexture.texture != nullptr)
	//	{
	//		emissiveTexture.texture = std::make_unique<Texture>(*aOther.emissiveTexture.texture);
	//	}
	//}
};

struct VertexMaterial
{
	std::string name = "";
	int id = -1;
};

struct VertexMaterialCollection
{
	TextureData vertex{};
	VertexMaterial r{};
	VertexMaterial g{};
	VertexMaterial b{};
	VertexMaterial a{};
};

struct VertexTextureCollection
{
	VertexMaterialCollection materials{};
	std::string meshName{};
	size_t subMeshIndex = 0; // used for debugging, can be removed
};
