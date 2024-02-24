#pragma once
#include "MenuComponent.h"
#include <string>

#include <engine/graphics/sprite/Sprite.h>

class Texture;
class MenuObject;

class SpriteComponent : public MenuComponent
{
public:
	SpriteComponent(MenuObject& aParent)
		: MenuComponent(aParent)
	{}

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

	SpriteInstanceData& GetInstanceData();
	Vector2f& GetPosition();
	Vector2f& GetSize();
	Vector2f& GetPivot();
	Vector2f& GetScaleMultiplier();
	Vector4f& GetColor();
	ClipValue& GetClipValue();
	float& GetRotation();
	bool& GetIsHidden();

	Texture* GetTexture() const;
	Vector2f GetTextureSize() const;
	std::string GetTexturePath();

	void SetTexture(Texture* aTexture, const std::string& aTextureName);
	void SetPosition(Vector2f aPosition);

private:
	SpriteInstanceData myInstance;
	SpriteSharedData mySharedData;
	std::string myTextureFile;

};
