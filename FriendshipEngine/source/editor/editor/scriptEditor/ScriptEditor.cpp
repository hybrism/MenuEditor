#include "ScriptEditor.h"

//External
#include <imnodes/imnodes_internal.h>
#include <imnodes/imnodes.h>
#include <imgui/imgui.h>
#include <filesystem>
#include <fstream>
#include <sstream>

//Engine
#include <engine/Paths.h>
#include <engine/utility/Error.h>
#include <engine/Defines.h>

//Game
#include <game/scripting/utility/ScriptJson.h>
#include <game/scripting/nodes/ScriptNodeTypeRegistry.h>
#include <game/scripting/ScriptStringRegistry.h>
#include <game/scripting/Script.h>
#include <game/scripting/ScriptManager.h>
#include <game/Game.h>
#include <game/utility/JsonUtility.h>

//Internal
#include <shared/postMaster/PostMaster.h>
#include <shared/command/CommandManager.h>
#include "commands/CreateLinkCommand.h"
#include "commands/CreateNodeCommand.h"
#include "commands/DestroyNodeAndLinksCommand.h"
#include "commands/FixUpSelectionCommand.h"
#include "commands/MoveNodesCommand.h"
#include "commands/ScriptEditorCommand.h"
#include "commands/SetOverriddenValueCommand.h"

#include "../editor/editor/friendshipEditor/EditorUpdateContext.h"

#ifdef max
#undef max
#endif

ScriptEditor::ScriptEditor()
{
}

ScriptEditor::~ScriptEditor()
{
}

void ScriptEditor::Init()
{
	FE::PostMaster::GetInstance()->Subscribe(this, FE::eMessageType::NewLevelLoaded);

	SCRIPT_FILEPATH = RELATIVE_IMPORT_DATA_PATH + (std::string)"scripts/";
	LEVEL_FILEPATH = RELATIVE_IMPORT_PATH;

	//Iterate through levels
	std::filesystem::create_directory(LEVEL_FILEPATH);
	for (const auto& entry : std::filesystem::directory_iterator(LEVEL_FILEPATH))
	{
		if (entry.path().extension() != ".json")
			continue;

		if (entry.path().filename() == "Resources.json")
			continue;

		ScriptStringId level = ScriptStringRegistry::RegisterOrGetStringId(entry.path().filename().string());
		myLevels[ScriptStringRegistry::GetStringFromStringId(level)];
	}

	//Get scripts related to levels
	nlohmann::json scripts = JsonUtility::OpenJson(SCRIPT_FILEPATH + "Scripts.json");
	for (const auto& pair : myLevels)
	{
		nlohmann::json jsonLevels = scripts["levels"];

		for (size_t i = 0; i < jsonLevels.size(); i++)
		{
			std::string_view levelName = jsonLevels[i]["name"];
			if (levelName == pair.first)
			{
				for (size_t j = 0; j < jsonLevels[i]["scripts"].size(); j++)
				{
					ScriptStringId nameStringId = ScriptStringRegistry::RegisterOrGetStringId(jsonLevels[i]["scripts"][j]);
					myLevels[pair.first].push_back(ScriptStringRegistry::GetStringFromStringId(nameStringId));
				}
			}
		}

	}

	std::filesystem::create_directory(SCRIPT_FILEPATH);
	for (const auto& entry : std::filesystem::directory_iterator(SCRIPT_FILEPATH))
	{
		if (entry.path().extension() != ".json")
			continue;

		if (entry == SCRIPT_FILEPATH + "Scripts.json")
			continue;

		std::filesystem::path fileName = entry.path().stem();
		std::string name = fileName.generic_u8string() + ".json";
		Script* script = ScriptManager::GetEditableScript(name);

		if (!script)
		{
			PrintW("[ScriptEditor.cpp] Could not load Editable Script!");
			continue;
		}

		ScriptStringId nameStringId = ScriptStringRegistry::RegisterOrGetStringId(name.c_str());
		std::string_view nameStringView = ScriptStringRegistry::GetStringFromStringId(nameStringId);

		EditorScriptData data{ script, {}, ImNodes::EditorContextCreate() };
		data.latestSavedSequenceNumber = script->GetSequenceNumber();

		myScripts.insert({ nameStringView, data });
	}
}

ScriptNodeTypeId ShowNodeTypeSelectorForCategory(const ScriptNodeTypeRegistry::CategoryInfo& category)
{
	// TODO: tooltip

	ScriptNodeTypeId result = { ScriptNodeTypeId::InvalidId };

	for (const ScriptNodeTypeRegistry::CategoryInfo& childCategory : category.childCategories)
	{
		std::string_view name = ScriptStringRegistry::GetStringFromStringId(childCategory.name);

		if (ImGui::BeginMenu(name.data()))
		{
			ScriptNodeTypeId type = ShowNodeTypeSelectorForCategory(childCategory);
			if (type.id != ScriptNodeTypeId::InvalidId)
				result = type;

			ImGui::EndMenu();
		}
	}

	for (ScriptNodeTypeId type : category.nodeTypes)
	{
		std::string_view name = ScriptNodeTypeRegistry::GetNodeTypeShortName(type);
		if (ImGui::MenuItem(name.data()))
		{
			result = type;
		}
	}

	return result;
}

void ScriptEditor::Update(const EditorUpdateContext& aContext)
{
	aContext;
	std::string level(myActiveLevel);
	Print(level);

	if (ImGui::BeginChild("NodeEditor"))
	{
		static char newScriptName[32];
		bool createNewCalled = false;
		ImGui::SetNextItemWidth(200);

		//SelectLevel();
		SelectScript();
		createNewCalled = ImGui::Button("Create new script...");

		if (createNewCalled)
		{
			strncpy_s(newScriptName, "untitled", sizeof(newScriptName));
			newScriptName[sizeof(newScriptName) - 1] = '\0';
			ImGui::OpenPopup("Create new script");
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Create new script", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{

			ImGui::InputText("##", newScriptName, IM_ARRAYSIZE(newScriptName), ImGuiInputTextFlags_AutoSelectAll);
			ImGui::Separator();

			if (ImGui::Button("Create", ImVec2(120, 0)))
			{
				ScriptStringId scriptNameId = ScriptStringRegistry::RegisterOrGetStringId(newScriptName);
				std::string_view scriptName = ScriptStringRegistry::GetStringFromStringId(scriptNameId);

				if (myScripts.find(scriptName) != myScripts.end())
				{
					// just switch to script if one already exists with this filename
					// TODO: should probably present an error message/warning
					myActiveScript = scriptName;
				}
				else
				{
					ScriptManager::AddEditableScript(scriptName, std::make_unique<Script>());

					myScripts.insert({ scriptName, EditorScriptData{ScriptManager::GetEditableScript(scriptName), {}, ImNodes::EditorContextCreate()} });
					myOpenScripts.insert({ scriptName, EditorScriptData{ScriptManager::GetEditableScript(scriptName), {}, ImNodes::EditorContextCreate()} });
					myActiveScript = scriptName;
				}

				ImGui::CloseCurrentPopup();
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (!myActiveScript.empty())
		{
			EditorScriptData& activeScript = myScripts[myActiveScript];
			ImNodes::EditorContextSet(activeScript.nodeEditorContext);

			Script& script = *activeScript.script;

			SaveButton(activeScript);
			RevertButton(activeScript);
			UndoButton();
			RedoButton();

			for (ScriptNodeId currentNodeId = script.GetFirstNodeId(); currentNodeId.id != ScriptNodeId::InvalidId; currentNodeId = script.GetNextNodeId(currentNodeId))
			{
				Vector2f pos = script.GetNodePosition(currentNodeId);
				ImNodes::SetNodeGridSpacePos(currentNodeId.id, { pos.x, pos.y });
			}

			// if a link is in progress, set default style color to match the link color
			// all regular links will have their colors overriden so this does not affect them
			if (activeScript.inProgressLinkPin.id != ScriptPinId::InvalidId)
			{
				// TODO: check pin node also
				// if it is wrong type, highlight link as red

				const ScriptPin& pin = script.GetPin(activeScript.inProgressLinkPin);
				int typeIndex = (int)pin.dataType;

				ImNodesStyle& style = ImNodes::GetStyle();
				style.Colors[ImNodesCol_Link] = IM_COL32(ScriptLinkColors[typeIndex][0], ScriptLinkColors[typeIndex][1], ScriptLinkColors[typeIndex][2], 255);
				style.Colors[ImNodesCol_LinkSelected] = IM_COL32(ScriptLinkSelectedColors[typeIndex][0], ScriptLinkSelectedColors[typeIndex][1], ScriptLinkSelectedColors[typeIndex][2], 255);
				style.Colors[ImNodesCol_LinkHovered] = IM_COL32(ScriptLinkHoverColors[typeIndex][0], ScriptLinkHoverColors[typeIndex][1], ScriptLinkHoverColors[typeIndex][2], 255);
			}

			NodeEditor(activeScript, aContext);

			{
				int startedLinkPinId;
				if (ImNodes::IsLinkStarted(&startedLinkPinId))
				{
					activeScript.inProgressLinkPin = { (unsigned int)startedLinkPinId };
				}
				else
				{
					activeScript.inProgressLinkPin = { ScriptPinId::InvalidId };
				}
			}

			{
				int hoveredNodeId;
				if (ImNodes::IsNodeHovered(&hoveredNodeId))
				{
					activeScript.hoveredNode = { (unsigned int)hoveredNodeId };
				}
				else
				{
					activeScript.hoveredNode = { ScriptNodeId::InvalidId };
				}
			}

			for (ScriptNodeId currentNodeId = script.GetFirstNodeId(); currentNodeId.id != ScriptNodeId::InvalidId; currentNodeId = script.GetNextNodeId(currentNodeId))
			{
				Vector2f oldPos = script.GetNodePosition(currentNodeId);
				ImVec2 newPos = ImNodes::GetNodeGridSpacePos(currentNodeId.id);

				if (newPos.x != oldPos.x || newPos.y != oldPos.y)
				{
					if (myInProgressMove == nullptr)
					{
						myInProgressMove = std::make_shared<MoveNodesCommand>(script, activeScript.selection);
						CommandManager::DoCommand(myInProgressMove);
					}

					script.SetPosition(currentNodeId, { newPos.x, newPos.y });
					myInProgressMove->SetPosition(currentNodeId, oldPos, { newPos.x, newPos.y });
				}
			}

			// clear in progress move if dragging ends
			if (!ImGui::IsMouseDown(0) && myInProgressMove)
			{
				myInProgressMove = nullptr;
			}

			int startId, endId;
			if (ImNodes::IsLinkCreated(&startId, &endId))
			{
				ScriptPinId sourcePinId = { (unsigned int)startId };
				ScriptPinId targetPinId = { (unsigned int)endId };
				const ScriptPin& sourcePin = script.GetPin(sourcePinId);
				const ScriptPin& targetPin = script.GetPin(targetPinId);

				if (sourcePin.dataType == targetPin.dataType && sourcePin.dataType != ScriptLinkDataType::Unknown)
				{
					std::shared_ptr<CreateLinkCommand> command = std::make_shared<CreateLinkCommand>(script, activeScript.selection, ScriptLink{ sourcePinId,targetPinId });
					CommandManager::DoCommand(command);
				}
			}

			int linkId;
			if (ImNodes::IsLinkDestroyed(&linkId))
			{
				std::shared_ptr<DestroyNodeAndLinksCommand> command = std::make_shared<DestroyNodeAndLinksCommand>(script, activeScript.selection);
				command->Add(ScriptLinkId{ (unsigned int)linkId });
				CommandManager::DoCommand(command);
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Delete))
			{
				int numSelectedLinks = ImNodes::NumSelectedLinks();
				int numSelectedNodes = ImNodes::NumSelectedNodes();

				if (numSelectedLinks > 0 || numSelectedNodes > 0)
				{
					std::shared_ptr<DestroyNodeAndLinksCommand> command = std::make_shared<DestroyNodeAndLinksCommand>(script, activeScript.selection);

					if (numSelectedLinks > 0)
					{
						std::vector<int> selectedLinks;
						selectedLinks.resize(static_cast<size_t>(numSelectedLinks));
						ImNodes::GetSelectedLinks(selectedLinks.data());
						for (int i = 0; i < selectedLinks.size(); i++)
						{
							command->Add(ScriptLinkId{ (unsigned int)selectedLinks[i] });
						}
					}

					if (numSelectedNodes > 0)
					{
						static std::vector<int> selectedNodes;

						selectedNodes.resize(static_cast<size_t>(numSelectedNodes));
						ImNodes::GetSelectedNodes(selectedNodes.data());
						for (int i = 0; i < selectedNodes.size(); i++)
						{
							command->Add(ScriptNodeId{ (unsigned int)selectedNodes[i] });
						}
					}

					CommandManager::DoCommand(command);
				}
			}

			// on right click show add node UI
			{
				bool showAddNodeUI = ImGui::IsWindowHovered(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::IsMouseClicked(1); // right mouse button

				if (showAddNodeUI)
				{
					ImGui::OpenPopup("Node Type Selection");
				}

				if (ImGui::BeginPopup("Node Type Selection"))
				{
					const ImVec2 clickPos = ImGui::GetMousePosOnOpeningCurrentPopup() - GImNodes->CanvasOriginScreenSpace - ImNodes::EditorContextGet().Panning;

					const ScriptNodeTypeRegistry::CategoryInfo& category = ScriptNodeTypeRegistry::GetRootCategory();
					ScriptNodeTypeId typeToCreate = ShowNodeTypeSelectorForCategory(category);

					if (typeToCreate.id != ScriptNodeTypeId::InvalidId)
					{
						std::shared_ptr<CreateNodeCommand> command = std::make_shared<CreateNodeCommand>(script, activeScript.selection, typeToCreate, Vector2f{ clickPos.x, clickPos.y });
						CommandManager::DoCommand(command);
					}

					ImGui::EndPopup();
				}
			}

		}
	}
	ImGui::EndChild();

}

void ScriptEditor::SelectScript()
{
	if (myOpenScripts.empty())
	{
		ImGui::SetNextItemWidth(200);
		if (ImGui::BeginCombo("Select Script", "N/A"))
			ImGui::EndCombo();
		return;
	}

	EditorScriptData& activeScriptData = myOpenScripts[myActiveScript];
	activeScriptData;

	char nameText[128];

	//TODO TOVE Script: (* after unsaved scripts) Fix this when I figure out exaclty how it works
	sprintf_s(nameText, "%s", myActiveScript.data());
	//if (activeScriptData.latestSavedSequenceNumber == activeScriptData.script->GetSequenceNumber())
	//	sprintf_s(nameText, "%s", myActiveScript.data());
	//else
	//	sprintf_s(nameText, "%s*", myActiveScript.data());

	ImGui::SetNextItemWidth(200);
	if (ImGui::BeginCombo("Select Script", nameText))
	{
		for (const auto& pair : myOpenScripts)
		{
			bool isSelected = pair.first == myActiveScript;


			//TODO TOVE Script: (* after unsaved scripts) Fix this when I figure out exaclty how it works
			sprintf_s(nameText, "%s", pair.first.data());
			//if (pair.second.latestSavedSequenceNumber == pair.second.script->GetSequenceNumber())
			//	sprintf_s(nameText, "%s", pair.first.data());
			//else
			//	sprintf_s(nameText, "%s*", pair.first.data());

			if (ImGui::Selectable(nameText, isSelected))
			{
				myActiveScript = pair.first;
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}
}

void ScriptEditor::SaveButton(EditorScriptData& aActiveScript)
{
	Script& script = *aActiveScript.script;

	// TODO: keyboard bindings

	if (ImGui::Button("Save"))
	{
		// use GameAssetPath when saving to ensure we always save to the games data path
		std::string basePath = SCRIPT_FILEPATH;

		std::filesystem::path scriptPath = std::filesystem::path(basePath) / std::filesystem::path(myActiveScript).replace_extension(".json");

		ScriptManager::SaveLevelScript(myActiveLevel, myActiveScript);

		ScriptJson jsonData;
		script.WriteToJson(jsonData);

		std::ofstream out(scriptPath);
		out << jsonData.json;
		out.close();

		aActiveScript.latestSavedSequenceNumber = script.GetSequenceNumber();
	}
}

void ScriptEditor::RevertButton(EditorScriptData& aActiveScript)
{
	Script& script = *aActiveScript.script;

	ImGui::SameLine();

	if (ImGui::Button("Revert"))
	{
		std::string basePath = SCRIPT_FILEPATH;
		std::filesystem::path scriptPath = std::filesystem::path(basePath) / std::filesystem::path(myActiveScript).replace_extension(".json");

		// TODO: pop up a warning, this clears undo history.
		CommandManager::Clear();
		script.Clear();

		std::ifstream file(scriptPath);
		if (file.is_open())
		{
			ScriptJson data = { nlohmann::json::parse(file) };
			file.close();
			script.LoadFromJson(data);
		}

		aActiveScript.latestSavedSequenceNumber = 0;
	}
}

void ScriptEditor::UndoButton()
{
	ImGui::SameLine();

	ImGui::BeginDisabled(!CommandManager::CanUndo());

	if (ImGui::Button("Undo"))
	{
		CommandManager::Undo();
	}

	ImGui::EndDisabled();
}

void ScriptEditor::RedoButton()
{
	ImGui::SameLine();

	ImGui::BeginDisabled(!CommandManager::CanRedo());

	if (ImGui::Button("Redo"))
	{
		CommandManager::Redo();
	}

	ImGui::EndDisabled();
}

void ScriptEditor::NodeEditor(EditorScriptData& aActiveScript, const EditorUpdateContext& aContext)
{
	aContext;

	ImNodes::BeginNodeEditor();
	Script& script = *aActiveScript.script;

	for (ScriptNodeId currentNodeId = script.GetFirstNodeId(); currentNodeId.id != ScriptNodeId::InvalidId; currentNodeId = script.GetNextNodeId(currentNodeId))
	{
		// TODO: tooltip

		ImNodes::BeginNode(currentNodeId.id);

		bool isNodeHighlighted = ImNodes::IsNodeSelected(currentNodeId.id) || aActiveScript.hoveredNode == currentNodeId;

		{
			ImNodes::BeginNodeTitleBar();

			ScriptNodeTypeId dataTypeId = script.GetNodeType(currentNodeId);
			std::string_view string = ScriptNodeTypeRegistry::GetNodeTypeShortName(dataTypeId);
			ImGui::TextUnformatted(string.data());

			ImNodes::EndNodeTitleBar();
		}

		ImVec2 cursorPos = ImGui::GetCursorPos();

		float widthLeft = 100.f;
		size_t inCount;
		const ScriptPinId* inPins = script.GetInputPins(currentNodeId, inCount);

		if (inCount == 0)
			widthLeft = 0.f;

		for (int i = 0; i < inCount; i++)
		{
			ScriptPinId pinId = inPins[i];
			const ScriptPin& pin = script.GetPin(pinId);
			std::string_view string = ScriptStringRegistry::GetStringFromStringId(pin.name);
			const float labelWidth = ImGui::CalcTextSize(string.data()).x;

			widthLeft = std::max(widthLeft, labelWidth);
		}

		float widthRight = 0.f;

		size_t outCount;
		const ScriptPinId* outPins = script.GetOutputPins(currentNodeId, outCount);
		for (int i = 0; i < outCount; i++)
		{
			ScriptPinId pinId = outPins[i];
			const ScriptPin& pin = script.GetPin(pinId);
			std::string_view string = ScriptStringRegistry::GetStringFromStringId(pin.name);
			const float labelWidth = ImGui::CalcTextSize(string.data()).x;

			widthRight = std::max(widthRight, labelWidth);
		}

		if (widthLeft > 0.f && widthRight > 0.f)
			widthRight += 20.f;

		{
			for (int i = 0; i < inCount; i++)
			{
				ScriptPinId pinId = inPins[i];
				const ScriptPin& pin = script.GetPin(pinId);
				int typeIndex = (int)pin.dataType;

				if (isNodeHighlighted)
					ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(ScriptLinkSelectedColors[typeIndex][0], ScriptLinkSelectedColors[typeIndex][1], ScriptLinkSelectedColors[typeIndex][2], 255));
				else
					ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(ScriptLinkColors[typeIndex][0], ScriptLinkColors[typeIndex][1], ScriptLinkColors[typeIndex][2], 255));

				ImNodes::PushColorStyle(ImNodesCol_PinHovered, IM_COL32(ScriptLinkHoverColors[typeIndex][0], ScriptLinkHoverColors[typeIndex][1], ScriptLinkHoverColors[typeIndex][2], 255));

				if (typeIndex == (int)ScriptLinkDataType::Flow)
					ImNodes::BeginInputAttribute(pinId.id, ImNodesPinShape_TriangleFilled);
				else
					ImNodes::BeginInputAttribute(pinId.id, ImNodesPinShape_CircleFilled);

				std::string_view string = ScriptStringRegistry::GetStringFromStringId(pin.name);

				const float labelWidth = ImGui::CalcTextSize(string.data()).x;
				ImGui::TextUnformatted(string.data());

				size_t connectionCount;
				script.GetConnectedLinks(pinId, connectionCount);
				if (connectionCount == 0)
				{

					bool hasOverridenValue = !std::holds_alternative<std::monostate>(pin.overridenValue.data);
					ScriptLinkData pinCurrentValue = hasOverridenValue ? pin.overridenValue : pin.defaultValue;
					ImGui::PushItemWidth(std::max(20.f, widthLeft - labelWidth));

					switch (pin.dataType)
					{
					case ScriptLinkDataType::Bool:
					{
						bool value = std::get<bool>(pinCurrentValue.data);
						ImGui::SameLine();
						if (ImGui::Checkbox("##", &value) && value != std::get<bool>(pinCurrentValue.data))
						{
							CommandManager::DoCommand(std::make_shared<SetOverriddenValueCommand>(script, aActiveScript.selection, pinId, ScriptLinkData{ value }));
						}

						break;
					}
					case ScriptLinkDataType::Int:
					{
						int value = std::get<int>(pinCurrentValue.data);
						ImGui::SameLine();
						if (ImGui::InputInt("##", &value, 0) && value != std::get<int>(pinCurrentValue.data))
						{
							CommandManager::DoCommand(std::make_shared<SetOverriddenValueCommand>(script, aActiveScript.selection, pinId, ScriptLinkData{ value }));
						}
						break;
					}
					case ScriptLinkDataType::Float:
					{
						float value = std::get<float>(pinCurrentValue.data);
						ImGui::SameLine();
						if (ImGui::InputFloat("##", &value) && value != std::get<float>(pinCurrentValue.data))
						{
							CommandManager::DoCommand(std::make_shared<SetOverriddenValueCommand>(script, aActiveScript.selection, pinId, ScriptLinkData{ value }));
						}
						break;
					}
					case ScriptLinkDataType::String:
					{
						static char stringBuffer[256];

						ScriptStringId value = std::get<ScriptStringId>(pinCurrentValue.data);

						strncpy_s(stringBuffer, ScriptStringRegistry::GetStringFromStringId(value).data(), sizeof(stringBuffer));
						stringBuffer[sizeof(stringBuffer) - 1] = '\0';

						ImGui::SameLine();
						if (ImGui::InputText("##", stringBuffer, IM_ARRAYSIZE(stringBuffer)))
						{
							ScriptStringId newValue = ScriptStringRegistry::RegisterOrGetStringId(stringBuffer);
							CommandManager::DoCommand(std::make_shared<SetOverriddenValueCommand>(script, aActiveScript.selection, pinId, ScriptLinkData{ newValue }));
						}
						break;
					}
					}

					ImGui::PopItemWidth();
				}

				ImNodes::EndInputAttribute();

				ImNodes::PopColorStyle();
				ImNodes::PopColorStyle();
			}
		}

		ImGui::SetCursorPos(cursorPos);

		{
			for (int i = 0; i < outCount; i++)
			{
				ScriptPinId pinId = outPins[i];
				const ScriptPin& pin = script.GetPin(pinId);
				int typeIndex = (int)pin.dataType;

				if (isNodeHighlighted)
					ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(ScriptLinkSelectedColors[typeIndex][0], ScriptLinkSelectedColors[typeIndex][1], ScriptLinkSelectedColors[typeIndex][2], 255));
				else
					ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(ScriptLinkColors[typeIndex][0], ScriptLinkColors[typeIndex][1], ScriptLinkColors[typeIndex][2], 255));

				ImNodes::PushColorStyle(ImNodesCol_PinHovered, IM_COL32(ScriptLinkHoverColors[typeIndex][0], ScriptLinkHoverColors[typeIndex][1], ScriptLinkHoverColors[typeIndex][2], 255));

				if (typeIndex == (int)ScriptLinkDataType::Flow)
					ImNodes::BeginOutputAttribute(pinId.id, ImNodesPinShape_TriangleFilled);
				else
					ImNodes::BeginOutputAttribute(pinId.id, ImNodesPinShape_CircleFilled);

				std::string_view string = ScriptStringRegistry::GetStringFromStringId(pin.name);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + widthLeft + widthRight - ImGui::CalcTextSize(string.data()).x);

				ImGui::TextUnformatted(string.data());
				ImNodes::EndInputAttribute();


				ImNodes::PopColorStyle();
				ImNodes::PopColorStyle();
			}
		}

		ImNodes::EndNode();
	}

	for (ScriptLinkId linkId = script.GetFirstLinkId(); linkId.id != ScriptLinkId::InvalidId; linkId = script.GetNextLinkId(linkId))
	{
		const ScriptLink& link = script.GetLink(linkId);

		const ScriptPin& sourcePin = script.GetPin(link.sourcePinId);
		const ScriptPin& targetPin = script.GetPin(link.targetPinId);

		int typeIndex = (int)(sourcePin.dataType == targetPin.dataType ? sourcePin.dataType : ScriptLinkDataType::Unknown);
		ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(ScriptLinkColors[typeIndex][0], ScriptLinkColors[typeIndex][1], ScriptLinkColors[typeIndex][2], 255));
		ImNodes::PushColorStyle(ImNodesCol_LinkSelected, IM_COL32(ScriptLinkSelectedColors[typeIndex][0], ScriptLinkSelectedColors[typeIndex][1], ScriptLinkSelectedColors[typeIndex][2], 255));
		ImNodes::PushColorStyle(ImNodesCol_LinkHovered, IM_COL32(ScriptLinkHoverColors[typeIndex][0], ScriptLinkHoverColors[typeIndex][1], ScriptLinkHoverColors[typeIndex][2], 255));

		ImNodes::Link(linkId.id, link.sourcePinId.id, link.targetPinId.id);

		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
	}

	ImNodes::EndNodeEditor();
}

void ScriptEditor::RecieveMessage(const FE::Message& aMessage)
{
	switch (aMessage.myEventType)
	{
	case FE::eMessageType::NewLevelLoaded:
	{
		std::string newLevel = std::any_cast<std::string>(aMessage.myMessage);
		OnNewLevelLoaded(newLevel);
		break;
	}
	default:
		break;
	}
}

void ScriptEditor::OnNewLevelLoaded(const std::string& aLevelName)
{
	myActiveLevel = aLevelName;
	myActiveScript = "";

	myOpenScripts.clear();


	for (size_t i = 0; i < myLevels[myActiveLevel].size(); i++)
	{
		if (myActiveScript.empty())
			myActiveScript = myLevels[myActiveLevel][i];

		myOpenScripts[myLevels[myActiveLevel][i]] = myScripts[myLevels[myActiveLevel][i]];
	}

}
