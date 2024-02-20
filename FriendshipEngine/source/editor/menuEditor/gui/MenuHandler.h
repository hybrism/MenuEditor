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

class MenuHandler
{
public:
	MenuHandler();
	~MenuHandler();

	void Init(const std::string& aMenuFile);
	void Update();
	void Render();

	ME::ObjectManager myObjectManager;


private:
	std::string myName;
	std::stack<MenuState*> myStateStack;
	std::array<MenuState, MAX_STATES> myStates;

	void LoadFromJson(const std::string& aMenuFile);
	void SaveToJson();
};