#pragma once
#include <engine/graphics/sprite/Sprite.h>
#include <string>

class SpriteDrawer;

#define BACKGROUND_OPACITY { 0.f, 0.f, 0.f, 0.7f }

enum class eButtonType
{
	StartGame,
	LevelSelect,
	Settings,
	Credits,
	Back,
	Quit,
	Resume,
	Resolution,
	Volume,
	BackToMain,
	VillageLevel,
	SwampLevel,
	DeepSwampLevel,
	CaveLevel,
	BossLevel,
	Count
};

enum class eButtonTextureType
{
	Default,
	Hovered,
	Slot_03,
	Slot_04,
	Count
};

//Gui will have different textures for each element in P6
//this is for simplicity :)
class GuiSprite
{
public:
	void Render(SpriteDrawer& aSpriteDrawer);
	Vector2f GetTextureSize() const;

	SpriteInstanceData myInstance;
	SpriteSharedData mySharedData;
	std::string myTexturePath;
};

class GuiCollider
{
public:
	bool IsHovered(const Vector2f& aMousePosition);
	void ReadDataFromSprite(const GuiSprite& aSprite);
	void ReadDataFromSprite(const SpriteInstanceData& aSpriteInstance, const SpriteSharedData& aSharedData);

	void SetSize(const Vector2f& aSize);
	void SetPosition(const Vector2f& aPosition);

	bool myIsHovered;

private:
	void UpdateMinMax();

	Vector2f myMax;
	Vector2f myMin;
	Vector2f mySize;
	Vector2f myPosition;
};


