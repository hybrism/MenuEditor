#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <engine/math/Vector.h>
#include <ecs/entity/Entity.h>
#include "Window.h"

class World;
class Texture;

struct MeshComponent;
class SharedMesh;
class Texture;
namespace FE
{
	struct Brush
	{
		float size = 100.0f;
		float hardness = 0.5f;
	};

	struct Material
	{
		Texture* texture = nullptr;
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


	constexpr float BRUSH_SIZE = 250.0f;
	constexpr unsigned int TEXTURE_SIZE = 4096 / 4;
	class VertexPainterWindow : public WindowBase
	{
	private:
		typedef size_t MeshId;
		typedef std::vector<unsigned int> IntersectingVerts;
		typedef std::array<unsigned int, TEXTURE_SIZE * TEXTURE_SIZE>* VertexTexture;
	public:
		VertexPainterWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);
		~VertexPainterWindow();

		void Show(const EditorUpdateContext& aContext) override;

	private:
		bool GetScreenSpacePosition(
			const EditorUpdateContext& aContext,
			Vector2f& outRatio,
			Vector2i& outScreenSpacePosition
		);
		unsigned int ColorToUint(const Vector4f& aColor);
		//Vector4f UintToColor(unsigned int aColor);
		void SaveVertexColors(const EditorUpdateContext& aContext, size_t aMeshId, const size_t aMeshSubIndex);
		void UpdateVertexBinding(const EditorUpdateContext& aContext, size_t aMeshId, size_t aMeshSubIndex);
		void Save(const EditorUpdateContext& aContext, size_t aMeshId, const size_t aMeshSubIndex);
		std::string GetTexturePath(const EditorUpdateContext& aContext, const SharedMesh* aMesh);

		void UpdateIntersectingVerts(const EditorUpdateContext& aContext);
		std::string GetNameFromToolMode(VertexPainterToolMode aToolMode);


		void ObjectSelectionMode(const EditorUpdateContext& aContext);
		void VertexPaintingMode(const EditorUpdateContext& aContext);

		void ObjectSelectionModeOnIntersect(const EditorUpdateContext& aContext, VertexPaintingToolModeContext& aToolContext);
		void VertexPaintingModeOnIntersect(const EditorUpdateContext& aContext, VertexPaintingToolModeContext& aToolContext);
		
		void DrawAllSelectedMeshes(const EditorUpdateContext& aContext);
		void SaveVertexBindings(SharedMesh* aMesh, const TextureInfo aTextureInfo);

		TextureInfo GetTextureInfo(const EditorUpdateContext& aContext, const SharedMesh* aMesh, size_t aMeshSubIndex);

		Palette myPalette;
		Brush myBrush;
		VertexPainterToolMode myToolMode = VertexPainterToolMode::ObjectSelection;

		std::unordered_map<MeshId, std::vector<IntersectingVerts>> myIntersectingVerts;
		std::unordered_map<MeshId, std::vector<VertexTexture>> myTestVertexColors;
		std::vector<Entity> mySelectedEntities;
	};
}