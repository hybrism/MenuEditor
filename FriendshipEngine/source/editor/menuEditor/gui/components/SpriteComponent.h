#pragma once
#include "Component.h"

#include <engine/graphics/sprite/Sprite.h>

class Texture;

namespace ME
{
	class MenuObject;
	
	class SpriteComponent : public Component
	{
	public:
		SpriteComponent(MenuObject& aParent)
			: Component(aParent)
		{}

		virtual void Init() override;
		virtual void Update() override;
		virtual void Render() override;

		Vector2f GetPosition() const;
		Vector2f GetSize() const;
		Vector2f GetTextureSize() const; 

		void SetTexture(Texture* aTexture);
		void SetPosition(Vector2f aPosition);

	private:
		SpriteInstanceData myInstance;
		SpriteSharedData mySharedData;

	};
}