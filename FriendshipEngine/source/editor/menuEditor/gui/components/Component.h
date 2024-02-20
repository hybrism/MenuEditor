#pragma once

namespace ME
{
	class MenuObject;
	class Component
	{
	public:
		virtual void Init() {};
		virtual void Update() {};
		virtual void Render() {};

		MenuObject& GetParent() const { return myParent; }

	protected:
		Component(MenuObject& aParent) :
			myParent(aParent) {}
		~Component() {}

		MenuObject& myParent;

	};
}