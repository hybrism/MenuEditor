#pragma once

class MenuObject;
class MenuComponent
{
public:
	virtual void Init() {};
	virtual void Update() {};
	virtual void Render() {};

	MenuObject& GetParent() const { return myParent; }

protected:
	MenuComponent(MenuObject& aParent) :
		myParent(aParent) {}
	virtual ~MenuComponent() {}

	MenuObject& myParent;
};