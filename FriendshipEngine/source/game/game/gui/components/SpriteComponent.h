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

		Texture* GetTexture(ObjectState aType = ObjectState::Default) const;
		Vector4f& GetColor(ObjectState aType = ObjectState::Default);

		Vector2f GetTextureSize(ObjectState aType = ObjectState::Default) const;
		std::string GetTexturePath(ObjectState aType = ObjectState::Default);

		void SetPosition(const Vector2f& aPosition);
		void SetPivot(const Vector2f& aPivot);
		void SetSize(const Vector2f& aSize);
		void SetScaleMultiplier(const Vector2f& aScaleMultiplier);
		void SetClipValue(const ClipValue& aClipValue);
		void SetRotation(float aRotation);
		void SetIsHidden(bool aIsHidden);

		void SetTexture(Texture* aTexture, const std::string& aTextureName, ObjectState aState = ObjectState::Default);
		void SetColor(const Vector4f& aColor, ObjectState aState = ObjectState::Default);

	private:
		SpriteInstanceData myInstance;

		std::array<TextureData, (int)ObjectState::Count> myTextures;

		std::string myTextureFile;

	};

}