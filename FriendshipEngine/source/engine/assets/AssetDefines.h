#pragma once
#include <string>

struct Animation;
class AnimationPlayer;
class Texture;

struct AssetMetaData
{
	size_t id = 0;
	std::string path = "";
};

struct TextureData
{
	Texture* texture = nullptr;
	std::string path = "";
};

struct TextureCollection
{
	TextureData albedoTexture{};
	TextureData normalTexture{};
	TextureData materialTexture{};
	TextureData emissiveTexture{};
};
