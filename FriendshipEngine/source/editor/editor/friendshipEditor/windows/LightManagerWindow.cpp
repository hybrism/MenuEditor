#include "LightManagerWindow.h"
#include <engine/math/Vector.h>
#include <engine/Defines.h>
#include <game/Game.h>
#include <engine\graphics\Camera.h>


FE::LightManagerWindow::LightManagerWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{}

void FE::LightManagerWindow::Show(const EditorUpdateContext& aContext)
{
	if (!myData.isOpen)
		return;
	aContext;

	auto& lightManager = aContext.game->GetLightManager();
	auto& dLight = lightManager.GetDirectionalLight();
	auto& pData = aContext.game->GetPostProcess().GetBufferData();
	auto& lightData = lightManager.GetLightDataRef();
	static Vector3<float> rots = dLight.myEuler;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		if (ImGui::DragFloat("xRot", &rots.x))
		{
			dLight.myEuler.x = rots.x;
			dLight.myDirectionalLightCamera->GetTransform().SetEulerAngles(rots);
		}
		if (ImGui::DragFloat("yRot", &rots.y))
		{
			dLight.myEuler.y = rots.y;
			dLight.myDirectionalLightCamera->GetTransform().SetEulerAngles(rots);
		}
		ImGui::DragFloat("Directional Intensity", &dLight.myIntensity, 0.05f, 0.0f);
		ImGui::DragFloat("Ambient Intensity", &dLight.myAmbientIntensity, 0.05f, 0.0f);

		bool useShadows = lightData.useShadows == 0;
		if (ImGui::Checkbox("Use Shadows", &useShadows))
		{
			lightData.useShadows = (int)!useShadows;
		}
		ImGui::DragFloat("Shadow Bias", &lightData.shadowBias, 0.00001f, 0.0f, 1.0f, "%f");
		ImGui::DragFloat("Shadow Offset Scale", &lightData.shadowOffsetScale, 0.00001f, 0.0f, 1.0f, "%f");

		ImGui::Separator();

		ImGui::Text("Vignette");
		ImGui::DragFloat("Inner", &pData.vignetteInner, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Outer", &pData.vignetteOuter, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Strength", &pData.vignetteStrength, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Curvature", &pData.vignetteCurvature, 0.1f, 0.0f, 100.0f);
		ImGui::ColorEdit3("Color", &pData.vignetteColor.myValues[0]);

		ImGui::Text("Speed Lines");
		ImGui::DragFloat("Speed Lines Strength", &pData.speedLineRadiusAddition, 0.1f, -500.0f, 500.0f);

	}
	ImGui::End();
}
