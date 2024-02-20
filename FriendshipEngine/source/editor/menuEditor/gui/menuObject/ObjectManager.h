#pragma once
#include <vector>
#include "MenuObject.h"

namespace ME
{
	class ObjectManager
	{
		friend class MenuObjectHierarchy;
		friend class InspectorWindow;
	
	public:
		ObjectManager();

		void Update();
		void Render();

		MenuObject& CreateNew();


	private:
		//TODO: G�ra till en Array? Indexet �r ID:t
		std::vector<MenuObject> myObjects;

		unsigned int myIdCounter;
		size_t myLastObjectIndex;

	};
}