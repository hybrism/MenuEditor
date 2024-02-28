#pragma once
#include <string>
#include <map>
#include <vector>

class Texture;
namespace MENU
{
	struct Assets
	{
		std::vector<Texture*> textures;
		std::vector<std::string> textureIdToName;
		std::map<std::string, int> textureNameToId;
		std::vector<std::string> fontFiles;
		std::vector<std::string> saveFiles;
	};
}