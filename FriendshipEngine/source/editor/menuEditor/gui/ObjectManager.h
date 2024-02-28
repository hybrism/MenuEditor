#pragma once
#include <vector>
#include <memory>
#include <engine/math/Vector.h>

namespace MENU
{
	class MenuObject;
	class ObjectManager
	{
		friend class MenuEditor;
		friend class MenuHandler;

	public:
		ObjectManager();

		void Update();
		void Render();

		void CheckCollision(const Vector2f& aPosition);

		MenuObject& CreateNew(const Vector2f& aPosition = { 0.f, 0.f });
		MenuObject& GetObjectFromID(unsigned int aID);
		
		void RemoveObjectAtID(unsigned int aID);
		void MoveUpObjectAtID(unsigned int aID);
		void MoveDownObjectAtID(unsigned int aID);

		void ClearAll();

	private:
		std::vector<std::shared_ptr<MenuObject>> myObjects;
		unsigned int myObjectIdCounter;
		size_t myLastObjectIndex;

	};
}