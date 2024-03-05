#pragma once
#include <vector>
#include <memory>
#include "MenuUpdateContext.h"

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

		void CheckCollision(const Vector2f& aPosition, bool aIsPressed = false);

		MenuObject& CreateNew(unsigned int aID = UINT_MAX, const Vector2f& aPosition = { 0.f, 0.f });
		MenuObject& GetObjectFromID(unsigned int aID);
		MenuObject& GetObjectFromIndex(unsigned int aIndex);
		
		void RemoveObjectAtID(unsigned int aID);
		void RemoveObjectAtIndex(unsigned int aIndex);

		void ClearAll();

	private:
		std::vector<std::shared_ptr<MenuObject>> myObjects;
		unsigned int myObjectIdCounter;
		size_t myLastObjectIndex;

	};
}