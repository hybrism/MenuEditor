#pragma once
#include <string>
#include <vector>
#include <engine/math/Vector.h>
#include "Window.h"

class World;
class Texture;

namespace FE
{
	class VertexPainterWindow : public WindowBase
	{
	public:
		VertexPainterWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);


		void Show(const EditorUpdateContext& aContext) override;

	private:
		bool GetScreenSpacePosition(
			const EditorUpdateContext& aContext,
			Vector2f& outRatio,
			Vector2i& outScreenSpacePosition
		);
	};
}