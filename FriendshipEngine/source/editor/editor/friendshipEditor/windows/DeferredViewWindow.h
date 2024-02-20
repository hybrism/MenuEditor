#pragma once
#include <string>
#include <vector>

#include "Window.h"

class World;
class Texture;

namespace FE
{
	class DeferredViewWindow : public WindowBase
	{
	public:
		DeferredViewWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);


		void Show(const EditorUpdateContext& aContext) override;

	private:

	};
}