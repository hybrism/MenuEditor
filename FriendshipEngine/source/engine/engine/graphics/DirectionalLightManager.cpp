#include "pch.h"
#include "DirectionalLightManager.h"
//#include "GraphicsEngine.h"
//#include "../utility/Error.h"
//#include "../graphics/GraphicsData.h"
////#include "imgui/imgui.h"
//#include "../graphics/Camera.h"
//
//DirectionalLightManager::DirectionalLightManager()
//{
//
//}
//
//DirectionalLightManager::~DirectionalLightManager()
//{
//	delete myDirectionalLightCamera;
//}
//
//void DirectionalLightManager::Init(const Vector3f& aDirection, const Vector4f& aColor)
//{
//	auto* g = GraphicsEngine::GetInstance();
//	HRESULT result;
//
//	//Skapa DirLight Buffer
//	D3D11_BUFFER_DESC bufferDescription = { 0 };
//	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
//	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	bufferDescription.ByteWidth = sizeof(DirectionalLightBufferData);
//	result = g->GetDevice()->CreateBuffer(&bufferDescription, nullptr, &myDirectionalLightBuffer);
//
//	if (FAILED(result))
//	{
//		PrintE("Could not create Directional Light Buffer");
//	}
//	aDirection;
//
//	//Sätta upp kamera
//	myDirectionalLightCamera = new Camera;
//	myDirectionalLightCamera->SetProjectionMatrix(60.2f, (float)g->GetWindowDimensions().x, (float)g->GetWindowDimensions().y, 1.f, 10000.f);
//	myDirectionalLightCamera->SetOrtographicProjection(-2500, 2500, -2500, 2500, -7500.f, 5000.f); //-2500, 2500, -2500, 2500, -7500.f, 5000.f
//	myDirectionalLightCamera->SetPosition({ 10, 100, 10 });
//	myDirectionalLightCamera->SetRotation(aDirection);
//	myDirection = aDirection;
//
//	SetDirectionAndColor(aDirection, aColor);
//
//	myShadowMapDepthBuffer = DepthBuffer::Create(g->GetViewportDimensions());
//
//
//	//SetDirectionAndColor(aDirection, aColor); //Osäker om denna behövs?
//
//	//UpdateBuffer();
//}
//
//void DirectionalLightManager::SetDirectionAndColor(const Vector3f& aDirection, const Vector4f& aColor)
//{
//	myDirectionalLightCamera->SetRotation(aDirection);
//	myColor = aColor;
//}
//
//void DirectionalLightManager::SetDirection(const Vector3f& aDirection)
//{
//	myDirectionalLightCamera->SetRotation(aDirection);
//	myDirection = aDirection;
//}
//
//void DirectionalLightManager::SetColor(const Vector4f& aColor)
//{
//	myColor = aColor;
//}
//
//void DirectionalLightManager::SetColor(const Vector3f& aColor)
//{
//	myColor.x = aColor.x;
//	myColor.y = aColor.y;
//	myColor.z = aColor.z;
//	myColor.w = 1.f;
//}
//
//void DirectionalLightManager::BeginShadowRendering()
//{
//	auto* g = GraphicsEngine::GetInstance();
//	auto* context = g->GetContext();
//
//	context->ClearDepthStencilView(myShadowMapDepthBuffer.GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//	g->ChangeCurrentCamera(myDirectionalLightCamera);
//	myShadowMapDepthBuffer.SetAsActiveTarget();
//
//	UpdateBuffer();
//}
//
//void DirectionalLightManager::EndShadowRendering()
//{
//	auto* g = GraphicsEngine::GetInstance();
//	auto* context = g->GetContext();
//
//	g->ResetToPrimaryCamera();
//	context->OMSetRenderTargets(1, g->GetBackBuffer().GetAddressOf(), g->GetDepthBuffer().GetDepthStencilView());
//	context->RSSetViewports(1, &g->GetViewPort());
//	context->PSSetShaderResources(7, 1, myShadowMapDepthBuffer.mySRV.GetAddressOf());
//	UpdateBuffer();
//}
//
//void DirectionalLightManager::EndOfRenderPassReset()
//{
//	ID3D11ShaderResourceView* empty = nullptr;
//	GraphicsEngine::GetInstance()->GetContext()->PSSetShaderResources(7, 1, &empty);
//}
//
//void DirectionalLightManager::SetDirCamerasPos(const Vector3f& aPos)
//{
//	Vector3f aOffsetPos = {0,100,0 };
//	myDirectionalLightCamera->SetPosition(aPos + aOffsetPos);
//}
//
//void DirectionalLightManager::UpdateBuffer()
//{
//	auto* g = GraphicsEngine::GetInstance();
//	auto* context = g->GetContext();
//	D3D11_MAPPED_SUBRESOURCE mappedBuffer{};
//
//	auto result = context->Map(myDirectionalLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
//	if (FAILED(result))
//	{
//		PrintE("Could not map DiretionalLightBuffer [DiretionalLight.cpp]");
//	}
//
//	DirectionalLightBufferData* data = (DirectionalLightBufferData*)mappedBuffer.pData;
//
//	//Vector4<float> direction = { myLightDir[0],myLightDir[1] * -1.f ,myLightDir[2],myLightDir[3] };
//	//data->dLightDir = direction.GetNormalized();
//
//	Vector3f dir = myDirectionalLightCamera->GetForward();
//
//	data->dLightDir = { -dir.x, -dir.y, -dir.z, 1.0f };//{ myMatrix(3,1),myMatrix(3,2) * -1.f,myMatrix(3,3),myMatrix(3,4)};
//
//	data->dLightColor = myColor;
//
//
//	//camMat(1, 1) = -1.f;
//	//camMat(2, 2) = 0.f;
//	//camMat(3, 3) = 0.f;
//	//camMat(3, 2) = -1.f;
//
//	//camMat(4, 1) = 0;
//	//	camMat(4, 2) = 0;
//	//	camMat(4, 3) = 0;
//	//	camMat(4, 4) = 1.f;
//
//	Matrix4x4f camMat = myDirectionalLightCamera->GetModelMatrix();
//	Matrix4x4f camProjMat = myDirectionalLightCamera->GetProjectionMatrix();
//
//	//data->dLightShadowSpaceToWorldSpace =  camMat * camProjMat;
//	data->dLightShadowSpaceToWorldSpace = (camMat * camProjMat);
//
//	context->Unmap(myDirectionalLightBuffer.Get(), 0);
//
//	context->VSSetConstantBuffers(6, 1, myDirectionalLightBuffer.GetAddressOf());
//	context->PSSetConstantBuffers(6, 1, myDirectionalLightBuffer.GetAddressOf());
//}
//
//void DirectionalLightManager::DEBUG_ImGuiChangeValues()
//{
//	//static bool openDirLight = false;
//
//	//ImGui::Begin("FriendshipDebug");
//	//ImGui::BeginMenuBar();
//	//if (ImGui::BeginMenu("Edit"))
//	//{
//	//	if (ImGui::MenuItem("Open Directional Light Settings"))
//	//	{
//	//		//Toggle
//	//		openDirLight = !openDirLight;
//
//	//	}
//	//	ImGui::EndMenu();
//	//}
//	//ImGui::EndMenuBar();
//	//ImGui::End();
//
//	//if (openDirLight)
//	//{
//	//	ImGui::Begin("Directional Light Adjustments");
//
//	//	{
//	//		Vector4f color = myColor * 255.0f;
//	//		if (ImGui::ColorEdit4("Color", &color.myValues[0]))
//	//		{
//	//			myColor = color / 255.0f;
//	//			UpdateBuffer();
//	//		}
//	//	}
//
//	//	if (ImGui::DragFloat3("Direction", &myDirection.myValues[0], 0.5f, -360.f, 360.f))
//	//	{
//	//		myDirectionalLightCamera->SetRotation(myDirection);
//
//	//		UpdateBuffer();
//	//	}
//	//	
//	//	{
//	//		Vector3f pos = myDirectionalLightCamera->GetPosition();
//	//		if (ImGui::DragFloat3("Position", &pos.myValues[0], 0.5f))
//	//		{
//	//			myDirectionalLightCamera->SetPosition(pos);
//
//	//			UpdateBuffer();
//	//		}
//	//	}
//
//	//	ImGui::Image(myShadowMapDepthBuffer.mySRV.Get(), ImVec2(400, 300));
//
//	//	ImGui::End();
//	//}
//}
