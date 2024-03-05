#pragma once

namespace MENU
{
	enum class ComponentType
	{
		Sprite,
		Collider2D,
		Text,
		Command,
		Count
	};

	static const char* ComponentNames[] =
	{
		"SpriteComponent",
		"ColliderComponent",
		"TextComponent",
		"CommandComponent",
		"None"
	};
};