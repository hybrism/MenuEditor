#pragma once
#include <stack>
#include <string>
#include <engine/math/Vector.h>
#include <nlohmann/json_fwd.hpp>
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
	Vector4f JsonToColorVec(nlohmann::json aJson);
	Vector2f JsonToVec2(nlohmann::json aJson);

	//TODO: These are not needed in game
	void SaveToJson();
	nlohmann::json ColorVecToJson(const Vector4f& aVec);
	nlohmann::json Vec2ToJson(const Vector2f& aVec);

private:
	std::string myName;
	std::string myFileName;

	std::stack<MenuState*> myStateStack;
	std::vector<MenuState> myStates;

};