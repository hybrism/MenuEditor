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
#include <directxtex/DirectXTex.h>
#include <game/component/MetaDataComponent.h>
#include <fstream>
#include <engine/graphics/renderer/DebugRenderer.h>
//#include <DirectXTex>

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
	myIntersectingVerts.clear();
	myModifiedMeshes.clear();
	mySelectedEntities.clear();
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

void FE::VertexPainterWindow::OnOpen(const EditorUpdateContext& aContext)
{
	UpdateSceneDirectory(aContext);
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

void FE::VertexPainterWindow::SaveVertexColors(VertexTexture& aTexture, const EditorUpdateContext& aContext, size_t aMeshId, const size_t aMeshSubIndex)
{
	{
		const UINT width = TEXTURE_SIZE;
		const UINT height = TEXTURE_SIZE;

		DirectX::Image image{};
		image.format = DXGI_FORMAT_R8G8B8A8_UNORM;
		image.width = width;
		image.height = height;
		image.rowPitch = width * sizeof(uint32_t);
		image.slicePitch = image.rowPitch * height;
		image.pixels = new uint8_t[image.slicePitch];

		memcpy(image.pixels, aTexture->data(), image.slicePitch);


		auto* mesh = AssetDatabase::GetMesh(aMeshId).meshData[aMeshSubIndex];
		TextureInfo info = GetTextureInfo(aContext, mesh, aMeshSubIndex);

		{
			auto ws = StringHelper::s2ws(info.texturePath);

			// Remove read only attribute due to perforce
			{
				std::ifstream stream(ws.c_str());
				if (stream.good())
				{
					SetFileAttributes(ws.c_str(), GetFileAttributes(ws.c_str()) & ~FILE_ATTRIBUTE_READONLY);
				}
			}

			HRESULT hr = DirectX::SaveToDDSFile(
				image,
				DirectX::DDS_FLAGS_ALLOW_LARGE_FILES,
				ws.c_str()
			);

			if (FAILED(hr))
			{
				PrintE("Failed to save vertex texture: " + std::to_string(hr));
				return;
			}
		}

		delete[] image.pixels;

		UpdateMeshToCurrentPalette(mesh, info);
	}
}

void FE::VertexPainterWindow::WriteToTexture(const EditorUpdateContext& aContext, const SharedMesh* aMesh, const size_t aMeshSubIndex)
{
	auto& tdb = AssetDatabase::GetTextureDatabase();
	VertexTextureCollection& collection = tdb.GetVertexTextureRef(aMesh->GetVertexTextureId());
	//collection.materials.r.
	aContext;
	aMeshSubIndex;
	collection;
}

void FE::VertexPainterWindow::UpdateVertexBinding(const EditorUpdateContext&, size_t aMeshId, size_t aMeshSubIndex)
{
	int vertexIndex = AssetDatabase::GetMesh(aMeshId).meshData[aMeshSubIndex]->GetTextures().vertexPaintIndex;

	if (vertexIndex == -1) { return; }

	VertexTextureCollection& collection = AssetDatabase::GetTextureDatabase().GetVertexTextureRef(vertexIndex);

	collection.materials.vertex.texture = TextureFactory::CreateDDSTextureWithCPUAccess(collection.materials.vertex.path, collection.materials.vertex.stagingTexture);
}

void FE::VertexPainterWindow::Save(VertexTexture& aTexture, const EditorUpdateContext& aContext, size_t aMeshId, const size_t aMeshSubIndex)
{
	SaveVertexColors(aTexture, aContext, aMeshId, aMeshSubIndex); // TEMP
	UpdateVertexBinding(aContext, aMeshId, aMeshSubIndex);
	std::this_thread::sleep_for(std::chrono::milliseconds(10)); // without this a read access violation occurs
}

void FE::VertexPainterWindow::SaveAll(const EditorUpdateContext& aContext)
{
	if (myModifiedMeshes.size() == 0) { return; }

	auto& tdb = AssetDatabase::GetTextureDatabase();
	std::string sceneName = aContext.game->GetSceneManager().GetCurrentSceneName();

	if (!std::filesystem::exists(RELATIVE_VERTEX_TEXTURE_ASSET_PATH + sceneName + "/"))
	{
		std::filesystem::create_directory(RELATIVE_VERTEX_TEXTURE_ASSET_PATH + sceneName + "/");
	}

	VertexTexture texture = new std::array<unsigned int, TEXTURE_SIZE* TEXTURE_SIZE>();
	auto* context = GraphicsEngine::GetInstance()->DX().GetContext();

	// unbind just in case
	ID3D11ShaderResourceView* nullSRV = nullptr;
	context->VSSetShaderResources((UINT)VertexShaderTextureSlot::VertexColor, 1, &nullSRV);

	for (ModifiedMesh& meshData : myModifiedMeshes)
	{
		for (size_t index = 0; index < meshData.meshSubIndices.size(); ++index)
		{
			if (!meshData.meshSubIndices[index]) { continue; }

			SharedMesh* mesh = AssetDatabase::GetMesh(meshData.meshId).meshData[index];

			auto& vertexMaterial = AssetDatabase::GetTextureDatabase().GetVertexTextureRef(mesh->GetVertexTextureId()).materials.vertex;
			vertexMaterial.stagingTexture->CopyToStaging();

			{
				D3D11_TEXTURE2D_DESC desc;
				vertexMaterial.stagingTexture->myTexture->GetDesc(&desc);

				D3D11_MAPPED_SUBRESOURCE stagingResource;
				vertexMaterial.stagingTexture->Map(stagingResource);
				{
					memcpy(texture->data(), stagingResource.pData, static_cast<size_t>(desc.Width) * static_cast<size_t>(desc.Height) * sizeof(uint32_t));
				}
				vertexMaterial.stagingTexture->Unmap();
			}

			texture->at(0) = mesh->GetVertexCount(); // reserving 0, 0
			Save(texture, aContext, meshData.meshId, index);
		}
		meshData.meshSubIndices.clear();
	}


	VertexIndexCollection& collection = aContext.game->GetSceneManager().GetVertexPaintedIndexCollection();

	tdb.ReplaceBinaryVertexPaintedTextureFile(sceneName, collection);
	AssetDatabase::LoadVertexTextures(sceneName);

	delete texture;
	myModifiedMeshes.clear();
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
	if (InputManager::GetInstance()->IsLeftMouseButtonDown())
	{
		// TODO: CHANGE THIS TO SOME SORT OF EVENT
		UpdateSceneDirectory(aContext);

		for (auto& [ meshId, pair ] : myIntersectingVerts)
		{
			std::vector<IntersectingVerts>& intersectingVerts = pair.intersectingVerts; // each index represents meshSubIndex
			std::vector<IntersectingVertWorldPositions>& intersectingVertWorldPositions = pair.intersectingVertWorldPositions;

			int modfiedMeshIndex = -1;
			{
				for (size_t i = 0; i < myModifiedMeshes.size(); i++)
				{
					if (myModifiedMeshes[i].meshId == meshId)
					{
						modfiedMeshIndex = static_cast<int>(i);
						break;
					}
				}

				if (modfiedMeshIndex == -1)
				{
					myModifiedMeshes.push_back(ModifiedMesh{ meshId, {} });
					myModifiedMeshes.back().meshSubIndices.resize(intersectingVerts.size(), false);
					modfiedMeshIndex = static_cast<int>(myModifiedMeshes.size()) - 1;
				}
			}

			for (size_t meshSubIndex = 0; meshSubIndex < intersectingVerts.size(); meshSubIndex++)
			{
				myModifiedMeshes[modfiedMeshIndex].meshSubIndices[meshSubIndex] = true;

				SharedMesh* mesh = AssetDatabase::GetMesh(meshId).meshData[meshSubIndex];
				unsigned int vertexCount = mesh->GetVertexCount();

				std::vector<SRVWriteData> writeData;

				auto& tdb = AssetDatabase::GetTextureDatabase();

				if (tdb.TryGetVertexTextureIndex(mesh->GetFileName(), meshSubIndex) == -1)
				{
					VertexTexture texture = new std::array<unsigned int, TEXTURE_SIZE* TEXTURE_SIZE>();
					texture->at(0) = mesh->GetVertexCount(); // reserving 0, 0
					Save(texture, aContext, meshId, meshSubIndex);
					delete texture;
				}

				if (aToolContext.meshComponent.vertexPaintedIndex < 0) {
					VertexIndexCollection& collection = aContext.game->GetSceneManager().GetVertexPaintedIndexCollection();
					if (collection.indexCounter.size() < meshId + 1)
					{
						collection.indexCounter.resize(meshId + 1, 0);
					}

					aToolContext.meshComponent.vertexPaintedIndex = collection.indexCounter[meshId]++;
					collection.vertexIndices[aToolContext.entity] = aToolContext.meshComponent.vertexPaintedIndex;
				}

				for (size_t intersectingVertIndex = 0; intersectingVertIndex < intersectingVerts[meshSubIndex].size(); intersectingVertIndex++)
				{
					unsigned int step = vertexCount * aToolContext.meshComponent.vertexPaintedIndex + 1; // 0, 0 is reserved for the vertex count
					unsigned int uvLocation[2] = { step % TEXTURE_SIZE, step / TEXTURE_SIZE };

					unsigned int vertexIndex = intersectingVerts[meshSubIndex][intersectingVertIndex];
					uvLocation[0] += vertexIndex;
					uvLocation[1] += uvLocation[0] / TEXTURE_SIZE;
					uvLocation[0] %= TEXTURE_SIZE;

					size_t index = static_cast<size_t>(uvLocation[1]) * TEXTURE_SIZE + static_cast<size_t>(uvLocation[0]);
					Vector3f vertWorldPosition = intersectingVertWorldPositions[meshSubIndex][intersectingVertIndex];

					float distance = (vertWorldPosition - aToolContext.cursorWorldPosition).Length();
					float ratio = 1.0f - distance / myBrush.size;
					ratio += myBrush.hardness;
					ratio = std::clamp(ratio, 0.0f, 1.0f);

					Vector4f finalColor = {
						myPalette.color.x * myPalette.channelStrength[0] * ratio,
						myPalette.color.y * myPalette.channelStrength[1] * ratio,
						myPalette.color.z * myPalette.channelStrength[2] * ratio,
						myPalette.color.w * myPalette.channelStrength[3] * ratio
					};

					unsigned int uintColor = ColorToUint(finalColor);
					writeData.push_back(SRVWriteData{ (unsigned int)index, uintColor });

					UpdateMeshToCurrentPalette(mesh, GetTextureInfo(aContext, mesh, meshSubIndex));
				}

				// TEMP
				//Save(aContext, meshId, meshSubIndex);
				// TODO: WRITE TO SRV

				//{
				//	DirectX::Image image{};
				//	image.format = DXGI_FORMAT_R8G8B8A8_UNORM;
				//	image.width = (size_t)texture->GetWidth();
				//	image.height = (size_t)texture->GetHeight();
				//	image.rowPitch = image.width * sizeof(uint32_t);
				//	image.slicePitch = image.rowPitch * image.height;
				//	image.pixels = new uint8_t[image.slicePitch];

				//	memcpy(image.pixels, myVertexColors[meshId][meshSubIndex]->data(), image.slicePitch);

				//	
				//}

				if (writeData.size() > 0)
				{

					auto* ge = GraphicsEngine::GetInstance();
					auto* context = ge->DX().GetContext();

					// unbind just in case
					ID3D11ShaderResourceView* nullSRV = nullptr;
					context->VSSetShaderResources((UINT)VertexShaderTextureSlot::VertexColor, 1, &nullSRV);

					auto& vertexMaterial = tdb.GetVertexTextureRef(mesh->GetVertexTextureId()).materials.vertex;
					vertexMaterial.stagingTexture->CopyToStaging();

					D3D11_MAPPED_SUBRESOURCE mappedResource;
					HRESULT hr = context->Map(vertexMaterial.texture->GetTexture().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
					if (FAILED(hr))
					{
						PrintE("[TextureFactory.cpp] Failed to map texture: ");
						context->Unmap(vertexMaterial.texture->GetTexture().Get(), 0);
						return;
					}

					UINT* data = reinterpret_cast<UINT*>(mappedResource.pData);

					{
						D3D11_MAPPED_SUBRESOURCE stagingResource;
						D3D11_TEXTURE2D_DESC desc;
						vertexMaterial.stagingTexture->myTexture->GetDesc(&desc);
						if (!vertexMaterial.stagingTexture->Map(stagingResource))
						{
							PrintE("[TextureFactory.cpp] Failed to map staging texture: ");
							return;
						}

						{
							memcpy(data, stagingResource.pData, static_cast<size_t>(desc.Width) * static_cast<size_t>(desc.Height) * sizeof(uint32_t));
						}
						vertexMaterial.stagingTexture->Unmap();
					}

					for (size_t i = 0; i < writeData.size(); i++)
					{
						data[writeData[i].index] = writeData[i].pixel;
					}

					context->Unmap(vertexMaterial.texture->GetTexture().Get(), 0);
				}

				writeData.clear();
			}
		}
	}
}


//void Texture::WriteToSRV(SRVWriteData* aRgbaPixels, size_t aSize)
//{
//}

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

void FE::VertexPainterWindow::SetVertexMaterial(const size_t& aChannelIndex, VertexMaterial& aMaterial) const
{
	if (!myPalette.activeChannels[aChannelIndex]) { return; }

	aMaterial.name = myPalette.materials[aChannelIndex].name;
	aMaterial.id = static_cast<int>(AssetDatabase::GetTextureIndex(aMaterial.name));
}

void FE::VertexPainterWindow::UpdateVertexMaterialsToCurrentPalette(VertexMaterialCollection& outCollection) const
{
	SetVertexMaterial(0, outCollection.r);
	SetVertexMaterial(1, outCollection.g);
	SetVertexMaterial(2, outCollection.b);
	SetVertexMaterial(3, outCollection.a);
}

void FE::VertexPainterWindow::UpdateMeshToCurrentPalette(SharedMesh* aMesh, const TextureInfo& aInfo)
{
	auto& tdb = AssetDatabase::GetTextureDatabase();

	int vertexPaintIndex = aMesh->GetVertexTextureId();
	VertexTextureCollection c;
	VertexTextureCollection* collection = &c;

	if (vertexPaintIndex >= 0)
	{
		collection = &AssetDatabase::GetTextureDatabase().GetVertexTextureRef(static_cast<size_t>(vertexPaintIndex));
	}

	UpdateVertexMaterialsToCurrentPalette(collection->materials);

	if (vertexPaintIndex == -1)
	{
		collection->meshName = aMesh->GetFileName();

		collection->subMeshIndex = aInfo.meshSubIndex;
		collection->materials.vertex.path = aInfo.texturePath;
		collection->materials.vertex.texture = TextureFactory::CreateDDSTextureWithCPUAccess(collection->materials.vertex.path, collection->materials.vertex.stagingTexture);

		aMesh->SetVertexTextureId(tdb.CreateVertexTexture(*collection));
	}
}

void FE::VertexPainterWindow::UpdateSceneDirectory(const EditorUpdateContext& aContext)
{
	auto sceneName = aContext.game->GetSceneManager().GetCurrentSceneName();

	if (sceneName == myPreviousSceneName) { return;	}

	if (!std::filesystem::exists(RELATIVE_VERTEX_TEXTURE_ASSET_PATH + sceneName + "/"))
	{
		std::filesystem::create_directory(RELATIVE_VERTEX_TEXTURE_ASSET_PATH + sceneName + "/");
	}
	myPreviousSceneName = sceneName;
}

FE::TextureInfo FE::VertexPainterWindow::GetTextureInfo(const EditorUpdateContext& aContext, const SharedMesh* aMesh, size_t aMeshSubIndex)
{
	TextureInfo info;
	info.sceneName = aContext.game->GetSceneManager().GetCurrentSceneName();
	info.sceneName = info.sceneName.substr(0, info.sceneName.find_last_of('.'));
	info.folderPath = RELATIVE_VERTEX_TEXTURE_ASSET_PATH + info.sceneName + "/";

	info.texturePath = info.folderPath + aMesh->GetFileName() + "_" + std::to_string(aMeshSubIndex) + ".dds";
	info.meshSubIndex = aMeshSubIndex;

	return info;
}

Vector3f FE::VertexPainterWindow::GetVertexWorldPosition(const Vertex& aVertex, World* aWorld, Entity aEntityId)
{
	DirectX::XMMATRIX objectWorldTransform = aWorld->GetComponent<TransformComponent>(aEntityId).GetWorldTransform(aWorld, aEntityId);
	Vector3f objectWorldPosition = { objectWorldTransform.r[3].m128_f32[0], objectWorldTransform.r[3].m128_f32[1], objectWorldTransform.r[3].m128_f32[2] };

	// Convert transform to rotation matrix
	objectWorldTransform.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	objectWorldTransform.r[0].m128_f32[3] = 0.0f;
	objectWorldTransform.r[1].m128_f32[3] = 0.0f;
	objectWorldTransform.r[2].m128_f32[3] = 0.0f;

	DirectX::XMVECTOR newCoord = DirectX::XMVector3TransformCoord({ aVertex.position.x, aVertex.position.y, aVertex.position.z }, objectWorldTransform);

	objectWorldPosition.x += newCoord.m128_f32[0];
	objectWorldPosition.y += newCoord.m128_f32[1];
	objectWorldPosition.z += newCoord.m128_f32[2];

	return objectWorldPosition;
}


void FE::VertexPainterWindow::UpdateIntersectingVerts(const EditorUpdateContext& aContext)
{
	Vector2f screenSpaceRatio;
	Vector2i screenSpacePosition;
	if (GetScreenSpacePosition(aContext, screenSpaceRatio, screenSpacePosition))
	{
		auto* instance = GraphicsEngine::GetInstance();

		Vector3f cursorWorldPosition{};
		Entity entityId = 0;

		// World Position
		{
			auto& rt = instance->GetGBuffer().myRenderTargets[(int)GBufferTexture::WorldPosition];

			D3D11_MAPPED_SUBRESOURCE data;
			rt.Map(data);
			{
				Vector4f pos = GetColor(screenSpacePosition, data.RowPitch, reinterpret_cast<float*>(data.pData), sizeof(float));
				cursorWorldPosition = { pos.x, pos.y, pos.z };
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

		auto& renderer = GraphicsEngine::GetInstance()->GetDebugRenderer();

		myIntersectingVerts.clear();
		myIntersectingVerts.insert({ meshId, {} });
		myIntersectingVerts[meshId].intersectingVerts.resize(package.meshData.size());
		myIntersectingVerts[meshId].intersectingVertWorldPositions.resize(package.meshData.size());

		for (size_t meshIndex = 0; meshIndex < package.meshData.size(); meshIndex++)
		{
			SharedMesh* mesh = AssetDatabase::GetMesh(meshId).meshData[meshIndex];
			Vertex* verts = mesh->GetVertices();

			for (unsigned int i = 0; i < mesh->GetVertexCount(); i++)
			{
				auto& vert = verts[i];

				Vector3f vertPos = GetVertexWorldPosition(vert, aContext.world, entityId);

				float distance = (vertPos - cursorWorldPosition).Length();
				if (distance < myBrush.size)
				{
					myIntersectingVerts[meshId].intersectingVerts[meshIndex].push_back(i);
					myIntersectingVerts[meshId].intersectingVertWorldPositions[meshIndex].push_back(vertPos);

					renderer.DrawQuad(vertPos, vert.normal, { 100, 100 }, { 1.0f, 0.0f, 0.0f });
				}
			}
		}


		if (myIntersectingVerts.size() == 0)
		{
			return;
		}

		VertexPaintingToolModeContext toolContext{ cursorWorldPosition, entityId, meshComponent };

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

	static int temp[2] = { 1600, 900 };
	ImGui::DragInt2("##", &temp[0]);

	if (ImGui::Button("WOWWWWW"))
	{
		GraphicsEngine::GetInstance()->SetInternalResolution({ temp[0], temp[1] });
		aContext.game->GetPostProcess().Init();
		aContext.game->GetLightManager().Init();
	}
	ImGui::DragFloat("DEPTH", &GraphicsEngine::GetInstance()->GetDepthFadeKRef());
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
			ImGui::DragFloat("###brush_size", &myBrush.size, 1.0f, 0.0f, 100000.0f, "%.1f");
			ImGui::SameLine();
			ImGui::SliderFloat("###brush_hardness", &myBrush.hardness, 0.0f, 1.0f);
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
			if (ImGui::BeginCombo(id.c_str(), "", (int)ImGuiComboFlags_HeightLarge | (int)ImGuiComboFlags_CustomPreview))
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


	// Save options
	{
		if (ImGui::Button("Save All Changes"))
		{
			SaveAll(aContext);
		}
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
