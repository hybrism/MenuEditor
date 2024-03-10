#pragma once
#include <stack>
#include <string>
#include <nlohmann/json_fwd.hpp>

#include "IDManager.h"
#include "ObjectManager.h"
#include "MenuUpdateContext.h"

namespace MENU
{
	struct MenuState
	{ 
		~MenuState()
		{
			IDManager::GetInstance()->FreeID(id);
		}

		std::string name;
		std::vector<ID> objectIds;
		ID id;
	};

	class MenuHandler
	{
		friend class MenuEditor;

	public:
		MenuHandler();
		~MenuHandler();

		void Init(const std::string& aMenuFile);
		void Update(const MenuUpdateContext& aContext);
		void Render();
		
		void AddNewState(const std::string& aName);
		void PushState(ID aID);
		void PopState();

		MenuState& GetCurrentState();
		std::vector<MenuState>& GetAllStates();

		MenuObject& GetObjectFromID(ID aID);
		MenuObject& GetObjectFromIndex(ID aIndex);

		void RemoveObjectAtID(ID aID);
		void MoveUpObjectAtID(ID aID);
		void MoveDownObjectAtID(ID aID);

		MenuObject& CreateNewObject(const Vector2f& aPosition = { 0.f, 0.f });

		//TODO: Move these to a "MenuLoader"
		void LoadFromJson(const std::string& aMenuFile);
		Vector4f JsonToColorVec(nlohmann::json aJson);
		Vector2f JsonToVec2(nlohmann::json aJson);
		Vector2f JsonToScreenPosition(nlohmann::json aJson);

		//TODO: These are not needed in game
		void SaveToJson();
		nlohmann::json ColorVecToJson(const Vector4f& aVec);
		nlohmann::json Vec2ToJson(const Vector2f& aVec);
		nlohmann::json ScreenPositionToJson(const Vector2f& aPosition);

	private:
		ObjectManager myObjectManager;

		//TODO: Change types of these (Takes alot of memory)
		std::string myName;
		std::string myFileName;

		std::stack<MenuState*> myStateStack;
		std::vector<MenuState> myStates;

		Vector2f myRenderSize;
	};
}