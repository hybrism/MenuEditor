#pragma once

class ImGuiHandler
{
public:
	ImGuiHandler();
	~ImGuiHandler();

	static void Init();
	static void Update();
	static void Render();

	static void Destroy();

private:
	static void SetCustomStyle();
};