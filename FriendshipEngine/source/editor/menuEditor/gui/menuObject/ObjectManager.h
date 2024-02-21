#pragma once
#include <array>
#include "MenuObject.h"

class ObjectManager
{
	friend class MenuHandler;

public:
	ObjectManager();

	void Update();
	void Render();

	MenuObject& CreateNew();
	void ClearAll();

	//TODO: Not make it public (?)
	std::vector<MenuObject> myObjects;

private:
	unsigned int myIdCounter;
	size_t myLastObjectIndex;

};