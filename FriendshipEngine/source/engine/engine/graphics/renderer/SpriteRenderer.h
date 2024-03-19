#pragma once
#include "Renderer.h"
#include "../sprite/Sprite.h"
#include "../sprite/SpriteDrawer.h"
#include "../../text/TextService.h"

#ifdef DrawText
#undef DrawText
#endif

class SpriteRenderer
{
	struct Sprite2DRenderData
	{
		SpriteSharedData sharedData;
		SpriteInstanceData instance;
	};

	struct Sprite3DRenderData
	{
		SpriteSharedData sharedData;
		Sprite3DInstanceData instance;
	};

public:
	SpriteRenderer();
	~SpriteRenderer();

	void Init();
	void Render();

	void DrawSprite(const SpriteSharedData& aSharedData, const SpriteInstanceData& aInstance);
	void DrawSprite(const SpriteSharedData& aSharedData, const Sprite3DInstanceData& aInstance);
	void DrawText(Text* aText);

	SpriteBatchScope BeginBatch(const SpriteSharedData& aSharedData, bool aUseDefaultVertexShader = true);
	
	TextService* GetTextService() { return &myTextService; }

private: 
	std::vector<Sprite2DRenderData> my2DSpritesToRender;
	std::vector<Sprite3DRenderData> my3DSpritesToRender;
	std::vector<Text*> myTextsToRender;

	SpriteDrawer mySpriteDrawer;
	TextService myTextService;
};