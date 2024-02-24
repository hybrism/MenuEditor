#include "VertexPainterWindow.h"

#include <algorithm>
#include <engine/graphics/GraphicsEngine.h>
#include <imgui/imgui.h>
#include <engine/utility/Error.h>
#include <engine/utility/InputManager.h>
#include <engine/graphics/Camera.h>
#include "../EditorManager.h"

#ifndef _DEBUG
#pragma warning (push)
#pragma warning (disable: 4702)
#endif

static constexpr int MAGIC_NUMBER = 20;

FE::VertexPainterWindow::VertexPainterWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags)
	: WindowBase(aHandle, aOpen, aFlags)
{}

static size_t GetIndex(const Vector2i& aPos, UINT aPitch, unsigned long long aSize)
{
	UINT pitch = aPitch / (UINT)aSize;
	return static_cast<size_t>(aPos.y * (int)pitch + aPos.x * aSize);
}

static Vector4f GetColor(const Vector2i& aPos, UINT aPitch, float* data)
{
	size_t index = GetIndex(aPos, aPitch, sizeof(float));
	return { data[index + 0], data[index + 1], data[index + 2] , data[index + 3] };
}

void FE::VertexPainterWindow::Show(const EditorUpdateContext& aContext)
{
#ifndef _DEBUG
	return;
#endif

	aContext;

	if (!myData.isOpen)
		return;

	if (ImGui::Begin(myData.handle.c_str(), &myData.isOpen, myData.flags))
	{
		Vector2f screenSpaceRatio;
		Vector2i screenSpacePosition;
		if (GetScreenSpacePosition(aContext, screenSpaceRatio, screenSpacePosition))
		{
			auto* instance = GraphicsEngine::GetInstance();

			auto& depthBuffer = instance->GetDepthBuffer();
			depthBuffer;
			auto& rt = instance->GetGBuffer().myRenderTargets[(int)GBufferTexture::WorldPosition];
			//auto& rt = depthBuffer;
			D3D11_MAPPED_SUBRESOURCE data;
			rt.Map(data);

			float* depthData = reinterpret_cast<float*>(data.pData);
			UINT pitch = data.RowPitch / sizeof(float[4]); // number of floats per row

			depthData;
			pitch;
			//auto* camera = instance->GetCamera();

			auto index = GetIndex(screenSpacePosition, pitch, sizeof(float));
			index;
			//float depthValue = depthData[index];
			Vector4f colorValue = GetColor(screenSpacePosition, data.RowPitch, depthData);
			// normalized device coordinate space
			// https://ars.els-cdn.com/content/image/3-s2.0-B9780128006450500063-f06-01-9780128006450.jpg
			//float ndcDepth = depthValue * 2.0f - 1.0f;

			//Matrix4x4f inv = Matrix4x4f::GetFastInverse(camera->GetViewMatrix

			//Vector3f cameraSpaceObjectPosition = Vector3f(
			//	screenSpaceRatio.x * depthBuffer.GetSize().x,
			//	screenSpaceRatio.y * depthBuffer.GetSize().y,
			//	camera->GetFarPlane()
			//);
			//cameraSpaceObjectPosition.Normalize();
			//cameraSpaceObjectPosition *= camera->GetFarPlane() * depthValue;

			//auto vec = DirectX::XMVector4Transform(
			//	DirectX::XMVectorSet(cameraSpaceObjectPosition.x, cameraSpaceObjectPosition.y, cameraSpaceObjectPosition.z, 1),
			//	camera->GetViewMatrix()
			//);
			//Vector3f pos = { vec.m128_f32[0], vec.m128_f32[1], vec.m128_f32[2] };
			//pos;


			//Print(std::to_string(depthValue));
			Print(std::to_string(colorValue.x) + ", " + std::to_string(colorValue.y) + ", " + std::to_string(colorValue.z) + ", " + std::to_string(colorValue.w));
			//Print(std::to_string(screenSpacePosition.x) + ", " + std::to_string(screenSpacePosition.y));
			//Print(std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
			//Print(std::to_string(mousePos.x) + ", " + std::to_string(mousePos.y));

			rt.Unmap();
		}
	}
	ImGui::End();
}

bool FE::VertexPainterWindow::GetScreenSpacePosition(
	const EditorUpdateContext& aContext,
	Vector2f& outRatio,
	Vector2i& outScreenSpacePosition
)
{
	auto* im = InputManager::GetInstance();

	Vector2i mousePos = im->GetCurrentMousePositionVector2i();
	mousePos.x *= -1;
	mousePos.y += MAGIC_NUMBER; // magic number offset????? maybe this is the border???

	auto gameWindowRect = aContext.editorManager->GetGameWindowRect();

	bool result =
		mousePos.x >= gameWindowRect.vMinWin.x && mousePos.x <= gameWindowRect.vMaxWin.x &&
		mousePos.y >= gameWindowRect.vMinWin.y && mousePos.y <= gameWindowRect.vMaxWin.y;

	if (result)
	{
		auto* instance = GraphicsEngine::GetInstance();
		auto& depthBuffer = instance->GetDepthBuffer();

		outRatio = Vector2f(
			(static_cast<float>(mousePos.x) - gameWindowRect.vMinWin.x) / (static_cast<float>(gameWindowRect.vMaxWin.x) - gameWindowRect.vMinWin.x),
			(static_cast<float>(mousePos.y) - gameWindowRect.vMinWin.y) / (static_cast<float>(gameWindowRect.vMaxWin.y) - gameWindowRect.vMinWin.y)
		);

		outScreenSpacePosition.x = static_cast<int>(static_cast<float>(depthBuffer.GetSize().x) * outRatio.x);
		outScreenSpacePosition.y = static_cast<int>(static_cast<float>(depthBuffer.GetSize().y) * outRatio.y);
	}

	return result;
}

#ifndef _DEBUG
#pragma warning (pop)
#endif
