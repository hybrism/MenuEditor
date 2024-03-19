#include "pch.h"
#include "SpriteRenderer.h"

#include <engine/graphics/GraphicsEngine.h>

SpriteRenderer::SpriteRenderer()
{}

SpriteRenderer::~SpriteRenderer()
{}

void SpriteRenderer::Init()
{
	mySpriteDrawer.Init();
	myTextService.Init();
}

void SpriteRenderer::Render()
{
	GraphicsEngine* ge = GraphicsEngine::GetInstance();

	//3D SPRITES
	for (const Sprite3DRenderData& sprite : my3DSpritesToRender)
	{
		mySpriteDrawer.Draw(sprite.sharedData, sprite.instance);
	}

	//2D SPRITES
	RenderState renderState;
	renderState.blendState = BlendState::AlphaBlend;
	renderState.depthStencilState = DepthStencilState::ReadOnly;
	ge->SetRenderState(renderState);

	for (const Sprite2DRenderData& sprite : my2DSpritesToRender)
	{
		mySpriteDrawer.Draw(sprite.sharedData, sprite.instance);
	}

	//TEXT
	for (Text* text : myTextsToRender)
	{
		myTextService.Draw(*text);
	}

	my2DSpritesToRender.clear();
	my3DSpritesToRender.clear();
	myTextsToRender.clear();
}

void SpriteRenderer::DrawSprite(const SpriteSharedData& aSharedData, const SpriteInstanceData& aInstance)
{
	my2DSpritesToRender.push_back({ aSharedData, aInstance });
}

void SpriteRenderer::DrawSprite(const SpriteSharedData& aSharedData, const Sprite3DInstanceData& aInstance)
{
	my3DSpritesToRender.push_back({ aSharedData, aInstance });
}

void SpriteRenderer::DrawText(Text* aText)
{
	myTextsToRender.push_back(aText);
}

SpriteBatchScope SpriteRenderer::BeginBatch(const SpriteSharedData& aSharedData, bool aUseDefaultVertexShader)
{
	return mySpriteDrawer.BeginBatch(aSharedData, aUseDefaultVertexShader);
}
