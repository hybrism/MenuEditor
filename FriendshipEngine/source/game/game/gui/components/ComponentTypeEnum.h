#pragma once

namespace MENU
{
	enum class ObjectState
	{
		Default,
		Hovered,
		Pressed,
		Count
	};

	static const char* ObjectStates[] = 
	{ 
		"Default", 
		"Hovered", 
		"Pressed" 
	};

	enum class ComponentType
	{
		Sprite,
		Collider2D,
		Text,
		Command,
		Interactable,
		Count
	};

	static const char* ComponentNames[] =
	{
		"SpriteComponent",
		"ColliderComponent",
		"TextComponent",
		"CommandComponent",
		"InteractableComponent",
		"None"
	};
};