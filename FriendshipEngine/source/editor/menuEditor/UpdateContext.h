#pragma once
#include <vector>
#include <string>
#include <memory>

class Texture;
//class MenuHandler;

namespace ME
{
	struct UpdateContext
	{
		std::vector<Texture*> textures;
		std::vector<std::string> textureIDtoPath;
		//std::shared_ptr<MenuHandler> menuHandler;
	};
}