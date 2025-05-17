#pragma once
#include <application/Application.h>
#include <engine/math/Transform.h>

#include <engine/graphics/PostProcess.h>
#include <engine/graphics/Light/LightManager.h>

#include "ModelViewer.h"

class Engine;
//struct SharedMeshPackage;

class ModelViewerLauncher : public Application
{
public:
	ModelViewerLauncher();
	~ModelViewerLauncher() override;

	void Init(HINSTANCE hInstance, WNDPROC wndProc) override;
	void Update(const float& aDt, const double& aTotalTime) override;
	void Render();

private:
	bool BeginFrame() override;
	void EndFrame() override;

	Engine* myEngine = nullptr;
	ModelViewer myModelViewer;
	PostProcess myPostProcess;
	LightManager myLightManager;
};

namespace Utility
{
	bool IsFbx(std::wstring aPath);
}