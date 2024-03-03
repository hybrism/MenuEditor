#pragma once
#include <string>
#include <array>

#include "MenuComponent.h"
#include <engine/graphics/sprite/Sprite.h>

class Texture;

namespace MENU
{
	enum class TextureState
	{
		Default,
		Hovered,
		Pressed,
		Count
	};

	class MenuObject;
	class SpriteComponent : public MenuComponent
	{
		struct TextureData
		{
			SpriteSharedData shared;
			Vector4f color = { 1.f, 1.f, 1.f,1.f };
			std::string fileName;
		};

	public:
		SpriteComponent(MenuObject& aParent, unsigned int aID);

		virtual void Update(const MenuUpdateContext& aContext) override;
		virtual void Render() override;

		SpriteInstanceData GetInstanceData() const { return myInstance; }
		Vector2f GetPosition() const { return myPosition; }
		Vector2f& GetSize() { return myInstance.size; }
		Vector2f& GetPivot() { return myInstance.pivot; }
		Vector2f& GetScaleMultiplier() { return myInstance.scaleMultiplier; }
		ClipValue& GetClipValue() { return myInstance.clip; }
		float& GetRotation() { return myInstance.rotation; }
		bool& GetIsHidden() { return myInstance.isHidden; }

		Texture* GetTexture(TextureState aType = TextureState::Default) const;
		Vector4f& GetColor(TextureState aType = TextureState::Default);

		Vector2f GetTextureSize(TextureState aType = TextureState::Default) const;
		std::string GetTexturePath(TextureState aType = TextureState::Default);

		void SetPosition(const Vector2f& aPosition);
		void SetPivot(const Vector2f& aPivot);
		void SetSize(const Vector2f& aSize);
		void SetScaleMultiplier(const Vector2f& aScaleMultiplier);
		void SetClipValue(const ClipValue& aClipValue);
		void SetRotation(float aRotation);
		void SetIsHidden(bool aIsHidden);

		void SetTexture(Texture* aTexture, const std::string& aTextureName, TextureState aState = TextureState::Default);
		void SetColor(const Vector4f& aColor, TextureState aState = TextureState::Default);

	private:
		SpriteInstanceData myInstance;

		std::array<TextureData, (int)TextureState::Count> myTextures;

		std::string myTextureFile;

		TextureState myState;
	};

}