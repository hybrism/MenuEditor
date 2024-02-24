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

std::shared_ptr<FE::WindowBase> FE::WindowFactory::Create(ID aID)
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_None;

	switch (aID)
	{
	case FE::ID::SceneHierarchy:
	{
		return std::make_shared<FE::SceneHierarchyWindow>("Scene Hierarchy", true, flags);
		break;
	}
	case FE::ID::GameView:
	{
		flags = ImGuiWindowFlags_NoScrollbar;
		return std::make_shared<FE::GameViewWindow>("Game View", true, flags);
		break;
	}
	case FE::ID::EditorView:
	{
		flags = ImGuiWindowFlags_NoScrollbar;
		return std::make_shared<FE::EditorViewWindow>("Editor View", true, flags);
		break;
	}
	case FE::ID::Inspector:
	{
		return std::make_shared<FE::InspectorWindow>("Inspector", true, flags);
		break;
	}
	case FE::ID::AssetDatabase:
	{
		return std::make_shared<FE::AssetDatabaseWindow>("AssetDatabase", true, flags);
		break;
	}
	case FE::ID::DeferredView:
	{
		flags = ImGuiWindowFlags_AlwaysAutoResize;
		return std::make_shared<FE::DeferredViewWindow>("Deferred View", false, flags);
		break;
	}
	case FE::ID::Console:
	{
		return std::make_shared<FE::ConsoleWindow>("Console", true, flags);
		break;
	}
	case FE::ID::ScriptEditor:
	{
		return std::make_shared<FE::ScriptEditorWindow>("Script Editor", false, flags);
		break;
	}
	case FE::ID::VertexPainter:
	{
		return std::make_shared<FE::VertexPainterWindow>("Vertex Painter", false, flags);
		break;
	}
	case FE::ID::Count:
	default:
		break;
	}

	return nullptr;
}