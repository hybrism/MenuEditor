#pragma once
#include <string>
#include <array>

#include "MenuComponent.h"
#include <engine/graphics/sprite/Sprite.h>

class Texture;

namespace MENU
{
	class MenuObject;
	class SpriteComponent : public MenuComponent
	{
		enum class TextureState
		{
			Default,
			Hovered,
			Pressed,
			Count
		};

		struct TextureData
		{
			SpriteSharedData shared;
			std::string fileName;
		};

	public:
		SpriteComponent(MenuObject& aParent);

		virtual void Update() override;
		virtual void Render() override;

		SpriteInstanceData GetInstanceData() const { return myInstance; }
		Vector2f GetPosition() const { return myPosition; }
		Vector2f& GetSize() { return myInstance.size; }
		Vector2f& GetPivot() { return myInstance.pivot; }
		Vector2f& GetScaleMultiplier() { return myInstance.scaleMultiplier; }
		Vector4f& GetColor() { return myInstance.color; }
		ClipValue& GetClipValue() { return myInstance.clip; }
		float& GetRotation() { return myInstance.rotation; }
		bool& GetIsHidden() { return myInstance.isHidden; }

		Texture* GetTexture(TextureState aType = TextureState::Default) const;
		Vector2f GetTextureSize(TextureState aType = TextureState::Default) const;
		std::string GetTexturePath(TextureState aType = TextureState::Default);

		void SetPosition(const Vector2f& aPosition);
		void SetPivot(const Vector2f& aPivot);
		void SetSize(const Vector2f& aPosition);
		void SetScaleMultiplier(const Vector2f& aPosition);
		void SetColor(const Vector4f& aPosition);
		void SetClipValue(const ClipValue& aClipValue);
		void SetRotation(float aRotation);
		void SetIsHidden(bool aIsHidden);
		void SetTexture(Texture* aTexture, const std::string& aTextureName, TextureState aType = TextureState::Default);

	private:
		SpriteInstanceData myInstance;
		SpriteSharedData mySharedData;

		std::array<TextureData, (int)TextureState::Count> myTextures;

		std::string myTextureFile;

		TextureState myState;
	};

}