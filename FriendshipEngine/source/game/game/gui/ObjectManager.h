#pragma once

class MenuObject;
class ObjectManager
{
public:
	ObjectManager();

	void Update();
	void Render();

	MenuObject& CreateNew();
	void ClearAll();

	//TODO: Not make it public (?)
	std::vector<std::shared_ptr<MenuObject>> myObjects;

private:
	unsigned int myIdCounter;
	size_t myLastObjectIndex;

};