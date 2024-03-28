#pragma once
#include <memory>
#include <vector>

#include "MenuComponent.h"

namespace MENU
{
	class Interaction;

	enum class InteractionType
	{
		None,
		Drag,
		Clip,
		Hide,

		Count
	};

	static const char* InteractionTypes[] =
	{
		"None",
		"Drag",
		"Clip",
		"Hide"
	};

	class InteractableComponent : public MenuComponent
	{
	public:
		InteractableComponent(MenuObject& aParent, unsigned int aID);

		virtual void Update(const MenuUpdateContext& aContext) override;

		std::shared_ptr<Interaction> AddInteraction(std::shared_ptr<MenuComponent> aComponent, InteractionType aType = InteractionType::None);
		void RemoveInteraction();

		std::vector<std::shared_ptr<Interaction>> myInteractions;

	private:
	};

	// ----- INTERACTIONS
	class Interaction
	{
	public:
		Interaction() = delete;
		Interaction(std::shared_ptr<MenuComponent> aParent)
			: myParent(aParent)
			, myType(InteractionType::None)
		{}

		virtual void OnPressed(const MenuUpdateContext&) { __noop; }
		virtual void OnHovered(const MenuUpdateContext&) { __noop; }

		InteractionType myType;
		std::shared_ptr<MenuComponent> myParent;
		float myValue;
	};

	class DragInteraction : public Interaction
	{
	public:
		DragInteraction(std::shared_ptr<MenuComponent> aParent);

		virtual void OnPressed(const MenuUpdateContext& aContext) override;

		float myMin;
		float myMax;
	};

	class HideInteraction : public Interaction
	{
	public:
		HideInteraction(std::shared_ptr<MenuComponent> aParent);

		virtual void OnPressed(const MenuUpdateContext& aContext) override;

		bool myIsHidden;
	};

	class ClipInteraction : public Interaction
	{
	public:
		ClipInteraction(std::shared_ptr<MenuComponent> aParent);

		virtual void OnPressed(const MenuUpdateContext& aContext) override;

	};
}