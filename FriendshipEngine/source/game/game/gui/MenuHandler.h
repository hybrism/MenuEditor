#pragma once
#include <stack>

#include "ObjectManager.h"

struct MenuState
{
	unsigned int id;
	std::string name;
	std::vector<unsigned int> objects;
};

class TextureFactory;

class MenuHandler
{
public:
	MenuHandler();
	~MenuHandler();

	void Init(const std::string& aMenuFile, TextureFactory* aTextureFactory);
	void Update();
	void Render();

	ObjectManager myObjectManager;

	//TODO: Move these to a "MenuLoader"
	void LoadFromJson(const std::string& aMenuFile, TextureFactory* aTextureFactory);
	void SaveToJson();

private:
	std::string myName;
	std::string myFileName;

	std::stack<MenuState*> myStateStack;
	std::vector<MenuState> myStates;

};