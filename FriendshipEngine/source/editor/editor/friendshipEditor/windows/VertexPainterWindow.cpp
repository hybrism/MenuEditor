#include "VertexPainterWindow.h"

#include <algorithm>
#include <imgui/imgui.h>
#include "../EditorManager.h"

#include <engine/graphics/GraphicsEngine.h>
#include <engine/graphics/Camera.h>
#include <engine/graphics/model/SharedMesh.h>
#include <engine/utility/Error.h>
#include <engine/utility/InputManager.h>
#include <engine/Paths.h>

#include <ecs/World.h>
#include <game/component/MeshComponent.h>
#include <assets/AssetDatabase.h>
#include <game/component/TransformComponent.h>
#include <assets/TextureFactory.h>
#include <engine/utility/StringHelper.h>
#include <assets/BinaryReader/BinaryVertexPaintingFileFactory.h>
#include <assets/ShaderDatabase.h>
#include <assets/TextureDatabase.h>
#include <thread>
#include <game/Game.h>
#include <engine/graphics/Texture.h>
#include <imgui_internal.h>

static constexpr int MAGIC_NUMBER = 25;

FE::VertexPainterWindow::VertexPainterWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{
	myPalette.materials[0].channelName = "R";
	myPalette.materials[1].channelName = "G";
	myPalette.materials[2].channelName = "B";
	myPalette.materials[3].channelName = "A";
}

FE::VertexPainterWindow::~VertexPainterWindow()
{
	for (auto& pair : myTestVertexColors)
	{
		for (size_t i = 0; i < pair.first; i++)
		{
			delete pair.second[i];
		}
	}
	myTestVertexColors.clear();
}

static size_t GetIndex(const Vector2i& aPos, UINT aPitch, unsigned long long aSize)
{
	UINT pitch = aPitch / (UINT)aSize;
	return static_cast<size_t>(aPos.y * (int)pitch + aPos.x * aSize);
}

static Vector4f GetColor(const Vector2i& aPos, UINT aPitch, float* aData, unsigned long long aSize)
{
	size_t index = GetIndex(aPos, aPitch, aSize);
	return { aData[index + 0], aData[index + 1], aData[index + 2], aData[index + 3] };
}

static Entity GetEntityFromEffectTexture(const Vector2i& aPos, UINT aPitch, unsigned char* data)
{
	Entity entity = 0;
	int offset = (aPos.y * aPitch) + (aPos.x * 4); // R8G8B8A8_UNORM

	//unsigned char r = data[offset];
	//unsigned char g = data[offset + 1];
	unsigned char b = data[offset + 2];
	unsigned char a = data[offset + 3];

	entity = b;
	entity <<= 8;
	entity |= a;

	return entity;
}

void FE::VertexPainterWindow::Show(const EditorUpdateContext& aContext)
{
	aContext;

	if (!myData.isOpen)
		return;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		//if (myTestVertexColors.size() > 0)

		ImGui::BeginGroup();
		{
			if (ImGui::BeginTabBar("##tabs"))
			{
				for (size_t i = 0; i < static_cast<size_t>(FE::VertexPainterToolMode::Count); ++i)
				{
					FE::VertexPainterToolMode mode = static_cast<FE::VertexPainterToolMode>(i);
					std::string name = GetNameFromToolMode(mode);

					ImGuiTabItemFlags tab_flags = ImGuiTabItemFlags_NoReorder;
					if (ImGui::BeginTabItem(name.c_str(), (bool*)0, tab_flags))
					{
						myToolMode = mode;
						ImGui::EndTabItem();
					}
				}

				ImGui::EndTabBar();
			}
		}
		ImGui::EndGroup();

		switch (myToolMode)
		{
		case FE::VertexPainterToolMode::ObjectSelection:
			ObjectSelectionMode(aContext);
			break;
		case FE::VertexPainterToolMode::VertexPainting:
			VertexPaintingMode(aContext);
			break;
		default:
			break;
		}

		DrawAllSelectedMeshes(aContext);

		UpdateIntersectingVerts(aContext);
	}
	ImGui::End();
}

#include <directxtex/DirectXTex.h>
#include <game/component/MetaDataComponent.h>
//#include <DirectXTex>

void FE::VertexPainterWindow::SaveVertexColors(const EditorUpdateContext& aContext, size_t aMeshId, const size_t aMeshSubIndex)
{
	{
		const UINT width = TEXTURE_SIZE;
		const UINT height = TEXTURE_SIZE;

		// Define pixel data (RGBA format, 32 bits per pixel)
		uint32_t* pixelData = myTestVertexColors[aMeshId][aMeshSubIndex]->data();

		// Create a new image
		DirectX::Image image;
		image.format = DXGI_FORMAT_R8G8B8A8_UNORM;
		image.width = width;
		image.height = height;
		image.rowPitch = width * sizeof(uint32_t);
		image.slicePitch = image.rowPitch * height;
		image.pixels = new uint8_t[image.slicePitch];

		// Copy the pixel data into the image
		memcpy(image.pixels, pixelData, image.slicePitch);

		// Save the image to a DDS file

		// TODO: SEPERATE MATERIAL FROM MESH

		auto* mesh = AssetDatabase::GetMesh(aMeshId).meshData[aMeshSubIndex];
		TextureInfo info = GetTextureInfo(aContext, mesh, aMeshSubIndex);
		HRESULT hr = DirectX::SaveToDDSFile(
			image,
			DirectX::DDS_FLAGS_ALLOW_LARGE_FILES,
			StringHelper::s2ws(info.texturePath).c_str()
		);
		hr;

		SaveVertexBindings(mesh, info);
	}
}

void FE::VertexPainterWindow::UpdateVertexBinding(const EditorUpdateContext&, size_t aMeshId, size_t aMeshSubIndex)
{
	int vertexIndex = AssetDatabase::GetMesh(aMeshId).meshData[aMeshSubIndex]->GetTextures().vertexPaintIndex;

	if (vertexIndex == -1) { return; }

	VertexTextureCollection& collection = AssetDatabase::GetTextureDatabase().GetVertexTextureRef(vertexIndex);

	delete collection.materials.vertex.texture;
	collection.materials.vertex.texture = TextureFactory::CreateTexture(collection.materials.vertex.path, false, false);
}

void FE::VertexPainterWindow::Save(const EditorUpdateContext& aContext, size_t aMeshId, const size_t aMeshSubIndex)
{
	SaveVertexColors(aContext, aMeshId, aMeshSubIndex); // TEMP
	UpdateVertexBinding(aContext, aMeshId, aMeshSubIndex);
	std::this_thread::sleep_for(std::chrono::milliseconds(10)); // without this a read access violation occurs
}

std::string FE::VertexPainterWindow::GetTexturePath(const EditorUpdateContext& aContext, const SharedMesh* aMesh)
{
	std::string sceneName = aContext.game->GetSceneManager().GetCurrentSceneName();
	sceneName = sceneName.substr(0, sceneName.find_last_of('.'));
	std::string folderPath = RELATIVE_VERTEX_TEXTURE_ASSET_PATH + sceneName + "/";

	return folderPath + aMesh->GetMaterialName() + ".dds";
}

void FE::VertexPainterWindow::ObjectSelectionModeOnIntersect(const EditorUpdateContext& aContext, VertexPaintingToolModeContext& aToolContext)
{
	aContext;
	aToolContext;
	if (InputManager::GetInstance()->IsLeftMouseButtonPressed())
	{

	}
}

void FE::VertexPainterWindow::VertexPaintingModeOnIntersect(const EditorUpdateContext& aContext, VertexPaintingToolModeContext& aToolContext)
{
	// TODO: RENDER IS IN THE WRONG ORDER:( need to fetch the render order from the mesh instance data
	if (InputManager::GetInstance()->IsLeftMouseButtonPressed())
	{
		for (auto& pair : myIntersectingVerts)
		{
			MeshId meshId = pair.first;
			auto& verts = pair.second;

			// TODO: VERTEX COLORS ARE NEVER LOADED ON INIT
			if (myTestVertexColors.find(meshId) == myTestVertexColors.end())
			{
				myTestVertexColors.insert({ meshId, {} });
				myTestVertexColors[meshId].resize(verts.size());

				for (size_t j = 0; j < verts.size(); j++)
				{
					myTestVertexColors[meshId][j] = new std::array<unsigned int, TEXTURE_SIZE* TEXTURE_SIZE>();
					myTestVertexColors[meshId][j]->at(0) = AssetDatabase::GetMesh(meshId).meshData[j]->GetVertexCount(); // reserving 0, 0
				}
			}

			for (size_t meshSubIndex = 0; meshSubIndex < verts.size(); meshSubIndex++)
			{
				for (size_t intersectingVertIndex = 0; intersectingVertIndex < verts[meshSubIndex].size(); intersectingVertIndex++)
				{
					unsigned int vertexIndex = verts[meshSubIndex][intersectingVertIndex] + 1; // 0, 0 is reserved for the vertex count
					unsigned int step = AssetDatabase::GetMesh(meshId).meshData[meshSubIndex]->GetVertexCount() * aToolContext.meshComponent.renderOrder;
					unsigned int uvLocation[2] = { step % TEXTURE_SIZE, step / TEXTURE_SIZE };
					uvLocation[1] += static_cast<unsigned int>(vertexIndex) / TEXTURE_SIZE;
					uvLocation[0] += static_cast<unsigned int>(vertexIndex) % TEXTURE_SIZE;


					size_t index = static_cast<size_t>(uvLocation[1]) * TEXTURE_SIZE + static_cast<size_t>(uvLocation[0]);
					unsigned int& arrayLocation = myTestVertexColors[meshId][meshSubIndex]->at(index);
					arrayLocation = ColorToUint(myPalette.color);
				}

				// TEMP SAVE ON EVERY CLICK
				Save(aContext, meshId, meshSubIndex);
				auto* mesh = AssetDatabase::GetMesh(meshId).meshData[meshSubIndex];
				SaveVertexBindings(mesh, GetTextureInfo(aContext, mesh, meshSubIndex));
				UpdateVertexBinding(aContext, meshId, meshSubIndex);
			}
		}
	}
}

void FE::VertexPainterWindow::DrawAllSelectedMeshes(const EditorUpdateContext& aContext)
{
	ImGui::Separator();

	ImGui::Text("Selected Meshes");

	if (ImGui::BeginChild("Selected Entities"))
	{
		for (Entity entity : mySelectedEntities)
		{
			auto& data = aContext.world->GetComponent<MetaDataComponent>(entity);

			std::string displayName = "[" + std::to_string(entity) + "] " + data.name;

			ImGui::Selectable(displayName.c_str());
		}
	}
	ImGui::EndChild();
}

void FE::VertexPainterWindow::SaveVertexBindings(
	SharedMesh* aMesh,
	const TextureInfo aTextureInfo
)
{
	auto& tdb = AssetDatabase::GetTextureDatabase();

	int vertexPaintIndex = aMesh->GetVertexTextureId();
	VertexTextureCollection c;
	VertexTextureCollection* collection = &c;

	if (vertexPaintIndex >= 0)
	{
		collection = &AssetDatabase::GetTextureDatabase().GetVertexTextureRef(static_cast<size_t>(vertexPaintIndex));
	}

	{
		if (myPalette.activeChannels[0])
		{
			collection->materials.r.name = myPalette.materials[0].name;
			collection->materials.r.id = static_cast<int>(AssetDatabase::GetTextureIndex(collection->materials.r.name));
		}
		if (myPalette.activeChannels[1])
		{
			collection->materials.g.name = myPalette.materials[1].name;
			collection->materials.g.id = static_cast<int>(AssetDatabase::GetTextureIndex(collection->materials.g.name));
		}
		if (myPalette.activeChannels[2])
		{
			collection->materials.b.name = myPalette.materials[2].name;
			collection->materials.b.id = static_cast<int>(AssetDatabase::GetTextureIndex(collection->materials.b.name));
		}
		if (myPalette.activeChannels[3])
		{
			collection->materials.a.name = myPalette.materials[3].name;
			collection->materials.a.id = static_cast<int>(AssetDatabase::GetTextureIndex(collection->materials.a.name));
		}
	}

	if (vertexPaintIndex == -1)
	{
		collection->meshName = aMesh->GetFileName();

		collection->subMeshIndex = aTextureInfo.meshSubIndex;
		collection->materials.vertex.path = aTextureInfo.texturePath;
		collection->materials.vertex.texture = TextureFactory::CreateTexture(collection->materials.vertex.path, false, false);

		aMesh->SetVertexTextureId(tdb.CreateVertexTexture(*collection));
	}

	tdb.UpdateVertexPaintedTextures(aTextureInfo.sceneName);
	AssetDatabase::LoadVertexTextures(aTextureInfo.sceneName);
}

FE::TextureInfo FE::VertexPainterWindow::GetTextureInfo(const EditorUpdateContext& aContext, const SharedMesh* aMesh, size_t aMeshSubIndex)
{
	TextureInfo info;
	info.sceneName = aContext.game->GetSceneManager().GetCurrentSceneName();
	info.sceneName = info.sceneName.substr(0, info.sceneName.find_last_of('.'));
	info.folderPath = RELATIVE_VERTEX_TEXTURE_ASSET_PATH + info.sceneName + "/";
	if (!std::filesystem::exists(info.folderPath))
	{
		std::filesystem::create_directories(info.folderPath);
	}

	info.texturePath = info.folderPath + aMesh->GetFileName() + "_" + std::to_string(aMeshSubIndex) + ".dds";
	info.meshSubIndex = aMeshSubIndex;

	return info;
}

void FE::VertexPainterWindow::UpdateIntersectingVerts(const EditorUpdateContext& aContext)
{
	Vector2f screenSpaceRatio;
	Vector2i screenSpacePosition;
	if (GetScreenSpacePosition(aContext, screenSpaceRatio, screenSpacePosition))
	{
		auto* instance = GraphicsEngine::GetInstance();

		Vector3f worldPosition{};
		Entity entityId = 0;

		// World Position
		{
			auto& rt = instance->GetGBuffer().myRenderTargets[(int)GBufferTexture::WorldPosition];

			D3D11_MAPPED_SUBRESOURCE data;
			rt.Map(data);
			{
				Vector4f pos = GetColor(screenSpacePosition, data.RowPitch, reinterpret_cast<float*>(data.pData), sizeof(float));
				worldPosition = { pos.x, pos.y, pos.z };
			}
			rt.Unmap();
		}
		// EntityID
		{
			auto& rt = instance->GetGBuffer().myRenderTargets[(int)GBufferTexture::Effects];

			D3D11_MAPPED_SUBRESOURCE data;
			rt.Map(data);
			{
				entityId = GetEntityFromEffectTexture(screenSpacePosition, data.RowPitch, reinterpret_cast<unsigned char*>(data.pData));
			}
			rt.Unmap();
		}

		if (entityId == INVALID_ENTITY)
		{
			return;
		}

		MeshComponent& meshComponent = aContext.world->GetComponent<MeshComponent>(entityId);
		size_t meshId = meshComponent.id;
		const SharedMeshPackage& package = AssetDatabase::GetMesh(meshId);

		myIntersectingVerts.clear();
		myIntersectingVerts.insert({ meshId, {} });
		myIntersectingVerts[meshId].resize(package.meshData.size());
		for (size_t meshIndex = 0; meshIndex < package.meshData.size(); meshIndex++)
		{
			SharedMesh* mesh = AssetDatabase::GetMesh(meshId).meshData[meshIndex];
			Vertex* verts = mesh->GetVertices();

			float brushSize = BRUSH_SIZE;
			for (unsigned int i = 0; i < mesh->GetVertexCount(); i++)
			{
				auto& vert = verts[i];

				Vector3f vertPos = { vert.position.x, vert.position.y, vert.position.z };
				vertPos = vertPos + aContext.world->GetComponent<TransformComponent>(entityId).transform.GetPosition();

				float distance = (vertPos - worldPosition).Length();
				if (distance < brushSize)
				{
					myIntersectingVerts[meshId][meshIndex].push_back(i);
				}
			}
		}


		if (myIntersectingVerts.size() == 0)
		{
			return;
		}

		VertexPaintingToolModeContext toolContext{ entityId, meshComponent };

		switch (myToolMode)
		{
		case FE::VertexPainterToolMode::ObjectSelection:
			ObjectSelectionModeOnIntersect(aContext, toolContext);
			break;
		case FE::VertexPainterToolMode::VertexPainting:
			VertexPaintingModeOnIntersect(aContext, toolContext);
			break;
		default:
			break;
		}
		//TextureFactory::WriteDDSToFile("test.dds", reinterpret_cast<unsigned char*>(myTestVertexColors[meshId]->data()), TEXTURE_SIZE, TEXTURE_SIZE, 4);
	}
}

std::string FE::VertexPainterWindow::GetNameFromToolMode(VertexPainterToolMode aToolMode)
{
	switch (aToolMode)
	{
	case FE::VertexPainterToolMode::ObjectSelection:
		return "Object Selection";
	case FE::VertexPainterToolMode::VertexPainting:
		return "Vertex Painting";
	default:
		return "Unknown";
	}
}

void FE::VertexPainterWindow::ObjectSelectionMode(const EditorUpdateContext& aContext)
{
	aContext;
	ImGui::Text("To be implemented!!!");
}

void FE::VertexPainterWindow::VertexPaintingMode(const EditorUpdateContext& aContext)
{
	aContext;

	// Brush
	{
		ImGui::Text("Brush");

		
		{
			float padding = 15.0f;
			ImGui::Text("Size");
			ImGui::SameLine(ImGui::CalcItemWidth() + padding);
			ImGui::Text("Hardness");
		}
		{
			ImGui::DragFloat("###", &myBrush.size, 1.0f, 1.0f, 1000.0f);
			ImGui::SameLine();
			ImGui::SliderFloat("###", &myBrush.hardness, 0.0f, 1.0f);
		}
	}

	ImGui::Separator();

	// Palette selection
	{
		{
			ImGui::Dummy(ImVec2(0.0f, 0.0f));
			ImGui::SameLine(ImGui::CalcItemWidth() / 2.0f);
			ImGui::Text("Color Strength");
			ImGui::SameLine(ImGui::CalcItemWidth() * 1.5f + ImGui::CalcTextSize("Texture").x / 2.0f);
			ImGui::Text("Texture");
		}

		ImVec2 selectableSize(200.0f, 40.0f); // Adjust as needed
		ImVec2 comboImageSize(15.0f, 15.0f); // Adjust as needed
		ImVec2 imageSize(selectableSize.y, selectableSize.y); // Adjust as needed

		ImGui::Dummy(ImVec2(0.0f, 0.0f));
		for (size_t i = 0; i < 4; i++)
		{
			{
				if (myPalette.materials[i].texture == nullptr)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
				}

				if (ImGui::Checkbox(myPalette.materials[i].channelName.c_str(), &myPalette.activeChannels[i]))
				{
					myPalette.color.myValues[i] = static_cast<float>(myPalette.activeChannels[i]) * myPalette.channelStrength[i];
				}

				if (myPalette.materials[i].texture == nullptr)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleColor();
				}

				{
					ImGui::SameLine();

					std::string sliderId = "##slider_" + std::to_string(i);
					if (ImGui::SliderFloat(sliderId.c_str(), &myPalette.channelStrength[i], 0.0f, 1.0f))
					{
						myPalette.color.myValues[i] = static_cast<float>(myPalette.activeChannels[i]) * myPalette.channelStrength[i];
					}
				}

				ImGui::SameLine();
			}

			std::string id = "##" + myPalette.materials[i].channelName;
			ImVec2 combo_pos = ImGui::GetCursorScreenPos();
			if (ImGui::BeginCombo(id.c_str(), "", ImGuiComboFlags_HeightLarge | ImGuiComboFlags_CustomPreview))
			{
				auto& ref = AssetDatabase::GetTextureDatabase().GetTexturesRef();

				{
					ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
					if (ImGui::Selectable("(None)", myPalette.materials[i].index == i, 0, selectableSize))
					{
						myPalette.materials[i].name = "";
						myPalette.materials[i].texture = nullptr;
						myPalette.color.myValues[i] = false;
						myPalette.activeChannels[i] = false;
					}
					ImGui::PopStyleVar();
				}

				for (size_t j = 0; j < ref.size(); j++)
				{
					std::string materialName = StringHelper::GetMaterialNameFromPath(ref[j].albedoTexture.path);

					ImVec2 prePos = ImGui::GetCursorScreenPos();
					ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.75f, 0.5f));
					{
						if (ImGui::Selectable(materialName.c_str(), myPalette.materials[i].index == i, 0, selectableSize))
						{
							myPalette.materials[i].name = materialName;
							myPalette.materials[i].texture = ref[j].albedoTexture.texture;
							myPalette.materials[i].index = i;
							myPalette.color.myValues[i] = true;
							myPalette.activeChannels[i] = true;
						}
					}
					ImGui::PopStyleVar();

					ImVec2 postPos = ImGui::GetCursorScreenPos();
					ImGui::SetCursorScreenPos(prePos);
					{
						ImGuiStyle& style = ImGui::GetStyle();
						ImGui::SetCursorScreenPos(ImVec2(prePos.x + style.FramePadding.x, prePos.y));
						ImGui::Image(ref[j].albedoTexture.texture->GetShaderResourceView(), { selectableSize.y, selectableSize.y });
					}
					ImGui::SetCursorScreenPos(postPos);
				}
				ImGui::EndCombo();
			}

			ImVec2 backup_pos = ImGui::GetCursorScreenPos();
			ImGuiStyle& style = ImGui::GetStyle();
			ImGui::SetCursorScreenPos(ImVec2(combo_pos.x + style.FramePadding.x, combo_pos.y));
			if (myPalette.materials[i].texture != nullptr)
			{
				ImGui::Image(myPalette.materials[i].texture->GetShaderResourceView(), comboImageSize);
				ImGui::SameLine();
				ImGui::Text(myPalette.materials[i].name.c_str());
			}
			ImGui::SetCursorScreenPos(backup_pos);
		}
		ImGui::Dummy(ImVec2(0.0f, 0.0f));
	}
}

bool FE::VertexPainterWindow::GetScreenSpacePosition(
	const EditorUpdateContext& aContext,
	Vector2f& outRatio,
	Vector2i& outScreenSpacePosition
)
{
	auto* im = InputManager::GetInstance();

	Vector2i mousePos = im->GetTentativeMousePosition();
	mousePos.y += MAGIC_NUMBER; // magic number offset????? maybe this is the border???

	auto gameWindowRect = aContext.editorManager->GetGameWindowRect();

	bool result =
		mousePos.x >= gameWindowRect.vMinWin.x && mousePos.x <= gameWindowRect.vMaxWin.x &&
		mousePos.y >= gameWindowRect.vMinWin.y && mousePos.y <= gameWindowRect.vMaxWin.y;

	if (result)
	{
		auto* instance = GraphicsEngine::GetInstance();
		auto& depthBuffer = instance->GetDepthBuffer();

		outRatio = Vector2f(
			(static_cast<float>(mousePos.x) - gameWindowRect.vMinWin.x) / (static_cast<float>(gameWindowRect.vMaxWin.x) - gameWindowRect.vMinWin.x),
			(static_cast<float>(mousePos.y) - gameWindowRect.vMinWin.y) / (static_cast<float>(gameWindowRect.vMaxWin.y) - gameWindowRect.vMinWin.y)
		);

		outScreenSpacePosition.x = static_cast<int>(static_cast<float>(depthBuffer.GetSize().x) * outRatio.x);
		outScreenSpacePosition.y = static_cast<int>(static_cast<float>(depthBuffer.GetSize().y) * outRatio.y);
	}

	return result;
}

unsigned int FE::VertexPainterWindow::ColorToUint(const Vector4f& aColor)
{
	unsigned int result = 0;
	result |= static_cast<unsigned int>(aColor.w * 255) << 24;
	result |= static_cast<unsigned int>(aColor.z * 255) << 16;
	result |= static_cast<unsigned int>(aColor.y * 255) << 8;
	result |= static_cast<unsigned int>(aColor.x * 255);
	return result;
}
