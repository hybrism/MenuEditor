#pragma once
#include <string>
#include <vector>

#include <shared/postMaster/Observer.h>
#include "Window.h"

#include <ecs/World.h>

class World;
class Texture;

namespace FE
{
	class SceneHierarchyWindow : public WindowBase
	{
	public:
		SceneHierarchyWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);
		
		void Show(const EditorUpdateContext& aContext) override;

	private:
		std::string mySceneHierarchySearchWord;
		Entity mySelectedEntity;

		void PushInspectorWindow(Entity aEntity);
	};
}