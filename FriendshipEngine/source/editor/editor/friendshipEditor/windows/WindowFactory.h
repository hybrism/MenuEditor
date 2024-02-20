#pragma once
#include <memory>
#include "Window.h"

namespace FE
{
	class WindowFactory
	{
	public:
		static std::shared_ptr<WindowBase> Create(ID aID);
	};
}