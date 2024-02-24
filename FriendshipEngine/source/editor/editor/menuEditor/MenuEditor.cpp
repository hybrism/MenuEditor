//#include "MenuEditor.h"
//
//#include <imgui/imgui.h>
//#include <assets/AssetDatabase.h>
//#include <assets/TextureFactory.h>
//
//#include <engine/Paths.h>
//
//#include <game/Game.h>
//#include <game/stateStack/StateStack.h>
//#include <game/stateStack/MainMenuState.h>
//#include <game/stateStack/PauseMenuState.h>
//
//#include <game/gui/MenuHandler.h>
//#include <game/gui/menuItem/MenuItem.h>
//
//
//
//MenuEditor::MenuEditor()
//{
//}
//
//MenuEditor::~MenuEditor()
//{
//}
//
//void MenuEditor::Init(Game* aGame)
//{
//	myGame = aGame;
//}
//
//void MenuEditor::ViewMenuHandlerWindow()
//{
//	MenuHandler* menuHandler = nullptr;
//
//	switch (myGame->GetStateStack()->GetCurrentStateType())
//	{
//	case eStateType::MainMenu:
//	{
//		MainMenuState* state = reinterpret_cast<MainMenuState*>(myGame->GetStateStack()->GetCurrentState());
//		menuHandler = state->GetMenuHandler();
//		break;
//	}
//	case eStateType::PauseMenu:
//	{
//		PauseMenuState* state = reinterpret_cast<PauseMenuState*>(myGame->GetStateStack()->GetCurrentState());
//		menuHandler = state->GetMenuHandler();
//		break;
//	}
//	default:
//		return;
//		break;
//	}
//
//	if (menuHandler->myShowMenuEditor)
//	{
//		if (ImGui::Begin("MenuEditor", &menuHandler->myShowMenuEditor, ImGuiWindowFlags_AlwaysAutoResize))
//		{
//			// ----- DEBUG DATA
//			ImGui::SeparatorText("Debug data");
//			float screenSize[2] = { menuHandler->myRenderSize.x, menuHandler->myRenderSize.y };
//			float screenCenter[2] = { menuHandler->myScreenCenter.x, menuHandler->myScreenCenter.y };
//
//			ImGui::DragFloat2("Screen Size", screenSize, 1.f, 0.f, 0.f, "%.f", ImGuiSliderFlags_NoInput);
//			ImGui::DragFloat2("Screen Center", screenCenter, 1.f, 0.f, 0.f, "%.f", ImGuiSliderFlags_NoInput);
//
//			// ----- EDIT VALUES
//			ImGui::SeparatorText("Edit values");
//
//			if (ImGui::TreeNode("Background"))
//			{
//				float bgPos[2] = { menuHandler->myBackground.myInstance.position.x, menuHandler->myBackground.myInstance.position.y };
//				float bgScale[2] = { menuHandler->myBackground.myInstance.scaleMultiplier.x, menuHandler->myBackground.myInstance.scaleMultiplier.y };
//
//				if (ImGui::DragFloat2("Background pos", bgPos))
//					menuHandler->myBackground.myInstance.position = { bgPos[0], bgPos[1] };
//
//				if (ImGui::DragFloat2("Background scale", bgScale, 0.01f))
//					menuHandler->myBackground.myInstance.scaleMultiplier = { bgScale[0], bgScale[1] };
//
//				ImGui::Text("Background sprite");
//				static char backgroundTex[CHAR_SIZE] = "";
//				menuHandler->SpriteTextureEdit(menuHandler->myBackground.mySharedData, backgroundTex);
//
//				ImGui::TreePop();
//			}
//
//			if (ImGui::TreeNode("Buttons"))
//			{
//				MenuState* current = menuHandler->GetCurrentMenuState();
//
//				if (ImGui::DragFloat("Top button height", &current->myTopButtonPosition.y))
//					menuHandler->ArrangeButtonPositions();
//
//				if (ImGui::DragFloat("Button spacing", &current->myButtonSpacing))
//					menuHandler->ArrangeButtonPositions();
//
//				if (ImGui::DragFloat("X-alignment", &current->myTopButtonPosition.x))
//					menuHandler->ArrangeButtonPositions();
//
//				float buttonScale[2] = { menuHandler->myButtonScale.x, menuHandler->myButtonScale.y };
//				if (ImGui::DragFloat2("Scale", buttonScale, 0.001f))
//				{
//					menuHandler->myButtonScale = { buttonScale[0], buttonScale[1] };
//					menuHandler->ArrangeButtonPositions();
//				}
//
//				for (int i = 0; i < menuHandler->GetCurrentButtons().size(); i++)
//				{
//					if (ImGui::TreeNode(menuHandler->GetCurrentButtons()[i]->GetTypeName().c_str()))
//					{
//						float pos[2] = { menuHandler->GetCurrentButtons()[i]->GetPosition().x, menuHandler->GetCurrentButtons()[i]->GetPosition().y };
//
//						ImGui::PushID(menuHandler->GetCurrentButtons()[i]->GetTypeName().c_str());
//						ImGui::Text("Button type: %s", menuHandler->GetCurrentButtons()[i]->GetTypeName().c_str());
//
//						static char defaultChar[CHAR_SIZE] = "";
//						static char hoveredChar[CHAR_SIZE] = "";
//
//						ImGui::Text("Default Sprite");
//						menuHandler->ButtonTextureEdit(eButtonTextureType::Default, defaultChar, i);
//
//						ImGui::Text("Hovered Sprite");
//						menuHandler->ButtonTextureEdit(eButtonTextureType::Hovered, hoveredChar, i);
//
//						ImGui::PopID();
//						ImGui::TreePop();
//					}
//				}
//				ImGui::TreePop();
//			}
//
//			if (ImGui::TreeNode("Sprites"))
//			{
//				static char spritePath[CHAR_SIZE] = "";
//				ImGui::InputTextWithHint("New Sprite", "Searches in MenuUI/...", spritePath, CHAR_SIZE);
//				if (ImGui::Button("Add new sprite"))
//				{
//					auto texFac = AssetDatabase::GetTextureFactory();
//					GuiSprite* newSprite = new GuiSprite();
//					std::string path = RELATIVE_SPRITE_ASSET_PATH + (std::string)"MenuUI/";
//					newSprite->mySharedData.texture = texFac->CreateTexture(path + spritePath);
//					if (newSprite->mySharedData.texture != nullptr)
//					{
//						newSprite->myInstance.position = menuHandler->myScreenCenter;
//						newSprite->myInstance.scale = newSprite->GetTextureSize();
//						newSprite->myTexturePath = spritePath;
//						menuHandler->GetCurrentMenuState()->mySprites.push_back(newSprite);
//					}
//					else
//					{
//						delete newSprite;
//					}
//				}
//
//				for (int i = 0; i < menuHandler->GetCurrentMenuState()->mySprites.size(); i++)
//				{
//					ImGui::PushID(i);
//					GuiSprite* sprite = menuHandler->GetCurrentMenuState()->mySprites[i];
//					ImGui::SeparatorText(sprite->myTexturePath.c_str());
//					float spritePos[2] = { sprite->myInstance.position.x, sprite->myInstance.position.y };
//					float spriteScale[2] = { sprite->myInstance.scaleMultiplier.x, sprite->myInstance.scaleMultiplier.y };
//
//					if (ImGui::DragFloat2("Sprite pos", spritePos))
//						sprite->myInstance.position = { spritePos[0], spritePos[1] };
//
//					if (ImGui::DragFloat2("Sprite scale", spriteScale, 0.01f))
//						sprite->myInstance.scaleMultiplier = { spriteScale[0], spriteScale[1] };
//
//					if (ImGui::Button("DELETE SPRITE"))
//					{
//						menuHandler->GetCurrentMenuState()->mySprites.erase(menuHandler->GetCurrentMenuState()->mySprites.begin() + i);
//						delete sprite;
//					}
//
//					ImGui::PopID();
//				}
//
//				ImGui::TreePop();
//			}
//
//			// ----- SAVE VALUES
//			ImGui::SeparatorText("Save new values");
//
//			if (ImGui::Button("Save"))
//				menuHandler->WriteMenuData(menuHandler->SAVE_FILE);
//			ImGui::SameLine();
//			if (ImGui::Button("Reset"))
//			{
//				menuHandler->ReadMenuData(menuHandler->SAVE_FILE);
//				for (int i = 0; i < static_cast<int>(eButtonType::Count); i++)
//				{
//					eButtonType type = static_cast<eButtonType>(i);
//					//vvv TODO TOVE: if we set textures beforehand buttons already know which texture it should have
//					//Maybe Set type in constructor aswell and remove init completely? hmm.
//					menuHandler->myButtons[type]->Init(menuHandler->myButtonTextures[type]);
//				}
//				menuHandler->ArrangeButtonPositions();
//			}
//
//		}
//			ImGui::End();
//	}
//}