#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>

class Texture;

namespace MENU
{
	struct Assets
	{
		std::vector<std::shared_ptr<Texture>> textures;
		std::vector<std::string> textureIdToName;
		std::map<std::string, int> textureNameToId;
		std::vector<std::string> fontFiles;
		std::vector<std::string> saveFiles;
	};
}