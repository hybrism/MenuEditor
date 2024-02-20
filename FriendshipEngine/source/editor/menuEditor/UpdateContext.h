#pragma once
#include <vector>

class Texture;
class MenuHandler;

namespace ME
{
	struct UpdateContext
	{
		std::vector<Texture*> textures;
		MenuHandler* menuHandler;
	};
}