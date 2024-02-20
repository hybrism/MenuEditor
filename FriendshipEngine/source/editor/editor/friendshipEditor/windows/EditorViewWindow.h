#pragma once
#include <string>
#include <vector>

#include "Window.h"

class World;
class Texture;

namespace FE
{
	class EditorViewWindow : public WindowBase
	{
	public:
		EditorViewWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);


		void Show(const EditorUpdateContext& aContext) override;

	private:

	};
}