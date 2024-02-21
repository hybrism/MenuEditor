#pragma once
#include <vector>
#include <string>

class Texture;
class MenuHandler;

namespace ME
{
	struct UpdateContext
	{
		std::vector<Texture*> textures;
		std::vector<std::string> textureIDtoPath;
		MenuHandler* menuHandler = nullptr;
	};
}