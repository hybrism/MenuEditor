#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <engine/math/Vector.h>
#include <ecs/entity/Entity.h>
#include "Window.h"

class World;
class Texture;

struct MeshComponent;
class SharedMesh;
class Texture;
struct Vertex;
struct VertexMaterial;
struct VertexMaterialCollection;

namespace FE
{
	constexpr float DEFAULT_BRUSH_SIZE = 250.0f;
	constexpr unsigned int TEXTURE_SIZE = 4096 / 4;

	typedef size_t MeshId;
	typedef std::vector<unsigned int> IntersectingVerts;
	typedef std::vector<Vector3f> IntersectingVertWorldPositions;
	typedef std::array<unsigned int, TEXTURE_SIZE* TEXTURE_SIZE>* VertexTexture;

	struct Brush
	{
		float size = 100.0f;
		float hardness = 1.0f;
	};

	struct Material
	{
		std::shared_ptr<Texture> texture = nullptr;
		std::string name = "";
		std::string channelName = "";
		size_t index = 0;
	};

	struct Palette
	{
		Vector4f color = { 0.0f, 0.0f, 0.0f, 0.0f };
		Material materials[4] = { {}, {}, {}, {} };
		float channelStrength[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		bool activeChannels[4] = { false, false, false, false };
	};

	struct VertexPaintingToolModeContext
	{
		Vector3f cursorWorldPosition;
		Entity entity;
		MeshComponent& meshComponent;
	};

	enum class VertexPainterToolMode
	{
		ObjectSelection,
		VertexPainting,
		Count
	};

	struct TextureInfo
	{
		std::string sceneName;
		std::string folderPath;
		std::string texturePath;
		size_t meshSubIndex;
	};

	struct IntersectionPair
	{
		std::vector<IntersectingVerts> intersectingVerts = {};  // each index represents meshSubIndex
		std::vector<IntersectingVertWorldPositions> intersectingVertWorldPositions = {};
	};

	struct ModifiedMesh
	{
		MeshId meshId;
		std::vector<bool> meshSubIndices;
	};

	class VertexPainterWindow : public WindowBase
	{
	public:
		VertexPainterWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);
		~VertexPainterWindow();

		void OnOpen(const EditorUpdateContext& aContext) override;
		void Show(const EditorUpdateContext& aContext) override;

	private:
		bool GetScreenSpacePosition(
			const EditorUpdateContext& aContext,
			Vector2f& outRatio,
			Vector2i& outScreenSpacePosition
		);
		unsigned int ColorToUint(const Vector4f& aColor);
		//Vector4f UintToColor(unsigned int aColor);
		void SaveVertexColors(VertexTexture& aTexture, const EditorUpdateContext& aContext, size_t aMeshId, const size_t aMeshSubIndex);
		void WriteToTexture(const EditorUpdateContext& aContext, const SharedMesh* aMesh, const size_t aMeshSubIndex);
		void UpdateVertexBinding(const EditorUpdateContext& aContext, size_t aMeshId, size_t aMeshSubIndex);
		void Save(VertexTexture& aTexture, const EditorUpdateContext& aContext, size_t aMeshId, const size_t aMeshSubIndex);
		void SaveAll(const EditorUpdateContext& aContext);
		std::string GetTexturePath(const EditorUpdateContext& aContext, const SharedMesh* aMesh);

		void UpdateIntersectingVerts(const EditorUpdateContext& aContext);
		std::string GetNameFromToolMode(VertexPainterToolMode aToolMode);


		void ObjectSelectionMode(const EditorUpdateContext& aContext);
		void VertexPaintingMode(const EditorUpdateContext& aContext);

		void ObjectSelectionModeOnIntersect(const EditorUpdateContext& aContext, VertexPaintingToolModeContext& aToolContext);
		void VertexPaintingModeOnIntersect(const EditorUpdateContext& aContext, VertexPaintingToolModeContext& aToolContext);
		
		void DrawAllSelectedMeshes(const EditorUpdateContext& aContext);
		
		// Updartes input collection with materials currently active in the palette
		void SetVertexMaterial(const size_t& aChannelIndex, VertexMaterial& aMaterial) const;
		void UpdateVertexMaterialsToCurrentPalette(VertexMaterialCollection& outCollection) const;

		void UpdateMeshToCurrentPalette(SharedMesh* aMesh, const TextureInfo& aInfo);

		void UpdateSceneDirectory(const EditorUpdateContext& aContext);

		TextureInfo GetTextureInfo(const EditorUpdateContext& aContext, const SharedMesh* aMesh, size_t aMeshSubIndex);

		Vector3f GetVertexWorldPosition(const Vertex& aVertex, World* aWorld, Entity aEntityId);

		Palette myPalette;
		Brush myBrush;
		std::string myPreviousSceneName = "";
		VertexPainterToolMode myToolMode = VertexPainterToolMode::ObjectSelection;

		std::unordered_map<MeshId, IntersectionPair> myIntersectingVerts;
		std::vector<ModifiedMesh> myModifiedMeshes;
		//std::unordered_map<MeshId, std::vector<VertexTexture>> myVertexColors;
		std::vector<Entity> mySelectedEntities;
	};
}