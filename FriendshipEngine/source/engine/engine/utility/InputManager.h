#pragma once
#include <bitset>
#include "Windows.h"
#include "Windowsx.h"
#include <engine/math/Vector.h>
#include <engine/graphics/GraphicsEngine.h>
#include <directxtk/Mouse.h>
#include <memory>

class InputManager
{
public:
	inline static InputManager* CreateInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new InputManager();
		}
		return myInstance;
	}

	inline static InputManager* GetInstance()
	{
		return myInstance;
	}

	inline static void DestroyInstance()
	{
		if (myInstance != nullptr)
		{
			delete myInstance;
			myInstance = nullptr;
		}
	}
	void Init();
	bool IsKeyHeld(const int aKeyCode) const { return myCurrentState[aKeyCode] && myPreviousState[aKeyCode]; }
	bool IsKeyPressed(const int aKeyCode) const { return myCurrentState[aKeyCode] && !myPreviousState[aKeyCode]; }
	bool IsKeyReleased(const int aKeyCode) const { return !myCurrentState[aKeyCode] && myPreviousState[aKeyCode]; }

	int ScrollWheelValue() const { return myMouse->GetState().scrollWheelValue; }
	
	POINT GetCurrentMousePosition() const
	{
		auto state = myMouse->GetState();
		return { state.x, state.y };
	}

	Vector2i GetCurrentMousePositionVector2i() const
	{
		auto state = myMouse->GetState();
		return Vector2i(-state.x, state.y);
	}

	Vector2f GetCurrentMousePositionVector2f() const
	{
		auto state = myMouse->GetState();
		return Vector2f((float)state.x, (float)state.y);
	}

	//void SetMouseMode(DirectX::Mouse::Mode aMode) { aMode; }
	void SetMouseMode(DirectX::Mouse::Mode aMode) { myMouse->SetMode(aMode); }

	bool IsScrollGoingUp() const { return myMouse->GetState().scrollWheelValue < 0; }
	bool IsScrollGoingDown() const { return myMouse->GetState().scrollWheelValue > 0; }

	//POINT GetTentativeMousePosition() const { return myTentativeMousePosition; }
	
	// For absolute mode, pixel location in x/y.
	// For relative mode, delta x / y.
	//POINT GetPreviousMousePosition() const { return myPreviousMousePosition; }
	//POINT GetMouseDelta() const { return myMouseDelta; }
	
	//Vector2<float> GetCurrentMousePositionFloat()
	//{
	//	auto viewportDimensions = GraphicsEngine::GetInstance()->GetViewportDimensions();
	//	return {
	//		static_cast<float>(myCurrentMousePosition.x),
	//		static_cast<float>(viewportDimensions.y - myCurrentMousePosition.y)
	//	};
	//}

	bool UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam); //, LPARAM lParam
	void LockMouseScreen(HWND mWindow);
	void UnlockMouseScreen();
	void Update();
private:
	InputManager();
	~InputManager();

	std::bitset<256> myTentativeState{};
	std::bitset<256> myCurrentState{};
	std::bitset<256> myPreviousState{};

	//POINT myTentativeMousePosition;
	//POINT myCurrentMousePosition;
	//POINT myPreviousMousePosition;

	//POINT myMouseDelta;

	//bool myMouseWheelStateUp;
	//bool myMouseWheelStateDown;
	std::unique_ptr<DirectX::Mouse> myMouse;
	static InputManager* myInstance;
};
