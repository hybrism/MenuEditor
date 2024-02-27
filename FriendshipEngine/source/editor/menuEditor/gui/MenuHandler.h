#pragma once
#include <stack>
#include <string>
#include <engine/math/Vector.h>
#include <nlohmann/json_fwd.hpp>
#include "ObjectManager.h"

class TextureFactory;

namespace MENU
{
	struct MenuState
	{
		std::string name;
		std::vector<unsigned int> objects;
		unsigned int id;
	};

	class MenuHandler
	{
		friend class MenuEditor;

	public:
		MenuHandler();
		~MenuHandler();

		void Init(const std::string& aMenuFile, TextureFactory* aTextureFactory);
		void Update();
		void Render();
		
		size_t GetObjectsSize();
		MenuObject& GetObjectFromID(size_t aID);

		void CreateNewObject(const Vector2f& aPosition = {0.f, 0.f});

		//TODO: Move these to a "MenuLoader"
		void LoadFromJson(const std::string& aMenuFile, TextureFactory* aTextureFactory);
		Vector4f JsonToColorVec(nlohmann::json aJson);
		Vector2f JsonToVec2(nlohmann::json aJson);

		//TODO: These are not needed in game
		void SaveToJson();
		nlohmann::json ColorVecToJson(const Vector4f& aVec);
		nlohmann::json Vec2ToJson(const Vector2f& aVec);

	private:
		ObjectManager myObjectManager;

		//TODO: Change types of these (Takes alot of memory)
		std::string myName;
		std::string myFileName;

		std::stack<MenuState*> myStateStack;
		std::vector<MenuState> myStates;

	};
}