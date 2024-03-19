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

	auto& dLight = aContext.game->GetLightManager().GetDirectionalLight();
	auto& pData = aContext.game->GetPostProcess().GetBufferData();
	static Vector3<float> rots = dLight.myEuler;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		if (ImGui::DragFloat("xRot", &rots.x))
		{
			aContext.game->GetLightManager().GetDirectionalLight().myEuler.x = rots.x;
			aContext.game->GetLightManager().GetDirectionalLight().myDirectionalLightCamera->GetTransform().SetEulerAngles(rots);
		}
		if (ImGui::DragFloat("yRot", &rots.y))
		{
			aContext.game->GetLightManager().GetDirectionalLight().myEuler.y = rots.y;
			aContext.game->GetLightManager().GetDirectionalLight().myDirectionalLightCamera->GetTransform().SetEulerAngles(rots);
		}
		if (ImGui::Button("Use Shadows"))
		{
			aContext.game->GetLightManager().myDoNotUseShadows = !aContext.game->GetLightManager().myDoNotUseShadows;
		} 
		ImGui::DragFloat("Directional Intensity", &dLight.myIntensity, 0.05f, 0.0f);
		ImGui::DragFloat("Ambient Intensity", &dLight.myAmbientIntensity, 0.05f, 0.0f);
	
		ImGui::Separator();

		ImGui::Text("Vignette");
		ImGui::DragFloat("Inner", &pData.vignetteInner, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Outer", &pData.vignetteOuter, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Strength", &pData.vignetteStrength, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Curvature", &pData.vignetteCurvature, 0.1f, 0.0f, 100.0f);
		ImGui::ColorEdit3("Color", &pData.vignetteColor.myValues[0]);

	}
	ImGui::End();
}
