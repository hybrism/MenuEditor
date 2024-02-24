#pragma once
#include <string>
#include <map>
#include <vector>

class Texture;
namespace MENU
{
	const std::string RELATIVE_MENUEDITOR_ASSETS = "../../content/menuEditor/";
	const std::string SPRITES = "sprites/";
	const std::string MENU_PATH = "menus/";
	const std::string FONT_PATH = "fonts/";

	struct Assets
	{
		std::vector<Texture*> textures;
		std::vector<std::string> textureIdToName;
		std::map<std::string, int> textureNameToId;
		std::vector<std::string> fontFiles;
		std::vector<std::string> saveFiles;
	};
}