#pragma once
#include <string>
#include <array>
#include <stack>

#include "menuObject/ObjectManager.h"

#define MAX_STATES 10

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
	std::array<MenuState, MAX_STATES> myStates;

};