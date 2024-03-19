#include "WindowFactory.h"

#include "AssetDatabaseWindow.h"
#include "ConsoleWindow.h"
#include "DeferredViewWindow.h"
#include "GameViewWindow.h"
#include "EditorViewWindow.h"
#include "InspectorWindow.h"
#include "SceneHierarchyWindow.h"
#include "ScriptEditorWindow.h"
#include "VertexPainterWindow.h"
#include "LightManagerWindow.h"

#include <engine/Defines.h>

std::shared_ptr<FE::WindowBase> FE::WindowFactory::Create(ID aID)
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_None;
	bool shouldOpen = false;

	switch (aID)
	{
	case FE::ID::SceneHierarchy:
	{
		shouldOpen = true;
		return std::make_shared<FE::SceneHierarchyWindow>("Scene Hierarchy", shouldOpen, flags);
		break;
	}
	case FE::ID::GameView:
	{
		flags = ImGuiWindowFlags_NoScrollbar;
		shouldOpen = true;

		return std::make_shared<FE::GameViewWindow>("Game View", shouldOpen, flags);
		break;
	}
	case FE::ID::EditorView:
	{
		flags = ImGuiWindowFlags_NoScrollbar;
		shouldOpen = true;

		return std::make_shared<FE::EditorViewWindow>("Editor View", shouldOpen, flags);
		break;
	}
	case FE::ID::Inspector:
	{
		shouldOpen = true;

		return std::make_shared<FE::InspectorWindow>("Inspector", shouldOpen, flags);
		break;
	}
	case FE::ID::AssetDatabase:
	{
		shouldOpen = true;

		return std::make_shared<FE::AssetDatabaseWindow>("AssetDatabase", shouldOpen, flags);
		break;
	}
	case FE::ID::DeferredView:
	{
		flags = ImGuiWindowFlags_AlwaysAutoResize;
		return std::make_shared<FE::DeferredViewWindow>("Deferred View", shouldOpen, flags);
		break;
	}
	case FE::ID::Console:
	{
		shouldOpen = true;

		return std::make_shared<FE::ConsoleWindow>("Console", shouldOpen, flags);
		break;
	}
	case FE::ID::ScriptEditor:
	{
		//flags = ImGuiWindowFlags_AlwaysAutoResize;

#if START_WITH_SCRIPTEDITOR
		shouldOpen = true;
#endif

		return std::make_shared<FE::ScriptEditorWindow>("Script Editor", shouldOpen, flags);
		break;
	}
	case FE::ID::VertexPainter:
	{
		flags = ImGuiWindowFlags_AlwaysAutoResize;
		return std::make_shared<FE::VertexPainterWindow>("Vertex Painter", shouldOpen, flags);
		break;
	}
	case FE::ID::DirectionalLight:
	{
		return std::make_shared<FE::LightManagerWindow>("Light Manager", false, flags);
		break;
	}
	case FE::ID::Count:
	default:
		break;
	}

	return nullptr;
}