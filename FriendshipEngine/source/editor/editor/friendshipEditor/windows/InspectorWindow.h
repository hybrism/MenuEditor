#pragma once
#include <string>
#include <vector>

#include <ecs/system/System.h>
#include <engine/math/Vector.h>
#include <shared/postMaster/Observer.h>
#include "Window.h"
#include "../inspector/Inspector.h"

class World;
class Texture;

namespace FE
{
	class InspectorWindow : public WindowBase, public Observer
	{
	public:
		InspectorWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);

		void Show(const EditorUpdateContext& aContext) override;
		
		void Clear();

	private:
		std::vector<Inspector> myInspectorWindows;
		int myWindowIdCounter = 0;

		void RecieveMessage(const Message& aMessage) override;
	};
}