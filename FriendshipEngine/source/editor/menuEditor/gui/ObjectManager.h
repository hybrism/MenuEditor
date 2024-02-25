#pragma once
#include <vector>
#include <memory>
#include <engine/math/Vector.h>

namespace MENU
{
	class MenuObject;
	class ObjectManager
	{
	public:
		ObjectManager();

		void Update();
		void Render();

		void CheckCollision(const Vector2f& aPosition);

		MenuObject& CreateNew(const Vector2f& aPosition = { 0.f, 0.f });
		
		void ClearAll();

		//TODO: Not make it public (?)
		std::vector<std::shared_ptr<MenuObject>> myObjects;

	private:
		unsigned int myIdCounter;
		size_t myLastObjectIndex;

	};
}