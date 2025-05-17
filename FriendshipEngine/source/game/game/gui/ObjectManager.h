#pragma once
#include <vector>
#include <memory>
#include "MenuUpdateContext.h"
#include "IDManager.h"

namespace MENU
{
	class MenuObject;
	class ObjectManager
	{
		friend class MenuEditor;
		friend class MenuHandler;

	public:
		ObjectManager();

		void Update(const MenuUpdateContext& aContext);
		void Render();

		void OnResize(const Vector2i& aNewRenderSize);

		MenuObject& CreateNew(ID aID = INVALID_ID, const Vector2f& aPosition = { 0.f, 0.f });
		MenuObject& GetObjectFromID(ID aID);
		MenuObject& GetObjectFromName(const std::string& aName);
		
		void RemoveObjectAtID(ID aID);

		void ClearAll();

	private:
		std::vector<std::shared_ptr<MenuObject>> myObjects;
		size_t myLastObjectIndex;

		void RemoveObjectAtIndex(unsigned int aIndex);
	};
}