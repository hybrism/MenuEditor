#pragma once
#include <application/Application.h>
#include <engine/math/Transform.h>

#include "ModelViewer.h"

class Engine;
//struct SharedMeshPackage;

class ModelViewerLauncher : public Application
{
public:
	ModelViewerLauncher();
	~ModelViewerLauncher() override;

	void Init(HINSTANCE hInstance, WNDPROC wndProc) override;
	void Update(const float& dt);
	void Render();

private:
	bool BeginFrame() override;
	void EndFrame() override;

	Engine* myEngine = nullptr;
	ModelViewer myModelViewer;
};

namespace MENU
{
	bool IsFbx(std::wstring aPath);
}