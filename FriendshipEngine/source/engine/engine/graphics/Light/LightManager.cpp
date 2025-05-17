#include "pch.h"

#include <engine/Defines.h>

#include "LightManager.h"
#include <assets\ModelFactory.h>
#include "..\DepthBuffer.h"
#include "..\Camera.h"
#include "..\model\Mesh.h"
#include "..\model\SharedMesh.h"
#include <assets\AssetDatabase.h>
#include <assets\ShaderDatabase.h>
#include "../../math/Transform.h"

#include "..\RenderTarget.h"
#include "..\GraphicsEngine.h"
#include "..\GBuffer.h"
#include "..\..\shaders\PixelShader.h"
#include "..\..\shaders\VertexShader.h"

#include "..\RenderDefines.h"
#include "..\renderer\ForwardRenderer.h"

#include <iostream>
LightManager::LightManager()
{
}

LightManager::~LightManager()
{
	delete myDirectionalLight->myDirectionalLightCamera;
	//delete myDirectionalLight->myShadowMapDepthBuffer;
	delete myDirectionalLight;


	myPointLights.clear();

	//delete myBoundingPixelShader;

}

void LightManager::ImportLights()
{
}

void LightManager::LightRender()
{

	ID3D11RenderTargetView* RTV = nullptr;
	ID3D11DepthStencilView* DSV = nullptr;
	auto* context = GraphicsEngine::GetInstance()->DX().GetContext();
	context->OMGetRenderTargets(1, &RTV, &DSV);

	ClearAllResources();
	RenderDirectionalLight();
	RenderPointLightsBounds();


	context->OMSetRenderTargets(1, &RTV, DSV);
}

void LightManager::Init(const Vector3f& aDirection, const Vector4f& aColor, const Vector3f& aPosition)
{
	auto* ge = GraphicsEngine::GetInstance();

#if !_MODELVIEWER
	myPointLights = AssetDatabase::GetPointLight();
	//AssetDatabase::ClearPointLight();

	aDirection;
	aColor;

	//myPointLights.clear();


	{   //   Directional Light
		//If intensity is less than 0, then there is no Directional Light.
		if (AssetDatabase::GetDirectionalLight().myIntensity > 0)
		{
			if (myDirectionalLight) {
				delete myDirectionalLight->myDirectionalLightCamera;
			}
			delete myDirectionalLight;
			myDirectionalLight = new DirectionalLight(AssetDatabase::GetDirectionalLight().myDirection, AssetDatabase::GetDirectionalLight().myColor, AssetDatabase::GetDirectionalLight().myIntensity);
			myDirectionalLight->myTransform = AssetDatabase::GetDirectionalLight().myTransform;
		}
		else
		{
			if (myDirectionalLight) {
				delete myDirectionalLight->myDirectionalLightCamera;
			}
			delete myDirectionalLight;
			std::cout << "[ERROR] [LightManager.cpp] There was no Directional Light to import, creating a hardcoded one ;) \n";
			myDirectionalLight = new DirectionalLight({ 1,1,0 }, { aColor.x, aColor.y, aColor.z }, 1);
			myDirectionalLight->myTransform.SetEulerAngles({ 45,45,45 });

		}
#else
		{
			if (myDirectionalLight) {
				delete myDirectionalLight->myDirectionalLightCamera;
		}
			delete myDirectionalLight;
			myDirectionalLight = new DirectionalLight(aDirection, { aColor.x, aColor.y, aColor.z }, aColor.w);
#endif

		
			Vector3<float> dirren;
			dirren.x = myDirectionalLight->myTransform.GetMatrix().r[2].m128_f32[0];
			dirren.y = myDirectionalLight->myTransform.GetMatrix().r[2].m128_f32[1];
			dirren.z = myDirectionalLight->myTransform.GetMatrix().r[2].m128_f32[2];
			myDirectionalLight->myDirection = dirren;
			myDirectionalLight->myDirectionalLightCamera = new Camera;
			myDirectionalLight->myDirectionalLightCamera->SetProjectionMatrix(60.2f, (float)ge->DX().GetWindowDimensions().x, (float)ge->DX().GetWindowDimensions().y, 100.f, 100000.f);
			myDirectionalLight->myDirectionalLightCamera->SetOrtographicProjection(-10000, 10000, -10000, 10000, -25000.f, 25000.f);
			myDirectionalLight->myDirectionalLightCamera->GetTransform().SetPosition(aPosition);
			myDirectionalLight->myDirectionalLightCamera->GetTransform().SetEulerAngles(myDirectionalLight->myTransform.GetEulerRotation());


#if _MODELVIEWER
			myDirectionalLight->myShadowMapDepthBuffer = DepthBuffer::Create({ 6000, 6000 });//g->GetViewportDimensions()
#else
			myDirectionalLight->myShadowMapDepthBuffer = DepthBuffer::Create({ 4096, 4096 }); // Hello, sänkte resolution så vi har 60fps i editor mode. Var 4096, 4096.
#endif

#ifdef _LAUNCHER
			myDirectionalLight->myShadowMapDepthBuffer = DepthBuffer::Create({ 8000, 8000 });//g->GetViewportDimensions()
#endif // 

#ifdef _RELEASE
			myDirectionalLight->myShadowMapDepthBuffer = DepthBuffer::Create({ 12000, 12000 });//g->GetViewportDimensions()
#endif // 


			myDirectionalLightRenderTarget = RenderTarget::Create(ge->DX().GetViewportDimensions(), DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT);
	}

#if !_MODELVIEWER

		{  //   PointLight
			myBoundingPixelShader = ShaderDatabase::GetPixelShader(PsType::LightBounds);
			myLightSphereMesh = AssetDatabase::GetMesh((int)PrimitiveMeshID::Sphere).meshData[0];

			myLightSphereMesh->SetVertexShader(ShaderDatabase::GetVertexShader(VsType::DefaultPBR));
			myLightSphereMesh->SetPixelShader(ShaderDatabase::GetPixelShader(PsType::DebugLine));
			myPointLightsRenderTarget = RenderTarget::Create(ge->DX().GetViewportDimensions(), DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
		}
#endif

		{   //   All of it
			HRESULT result;
			D3D11_BUFFER_DESC bufferDescription = { 0 };
			bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
			bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDescription.ByteWidth = sizeof(LightBufferData);
			result = ge->DX().GetDevice()->CreateBuffer(&bufferDescription, nullptr, &myLightBuffer);

			if (FAILED(result))
			{
				std::cout << (" \n [ERROR] [LightManager.cpp]Could not create Light Buffer  \n");
			}
		}

		SetLightsOnBuffer();
}

void LightManager::SetLightsOnBuffer()
{
	auto* g = GraphicsEngine::GetInstance();
	auto* context = g->DX().GetContext();
	D3D11_MAPPED_SUBRESOURCE mappedBuffer{};


	auto result = context->Map(myLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	if (FAILED(result))
	{
		std::cout << ("[ERROR] [LightManager.cpp] Could not LightBuffer \n");
	}

	{
		if (myPointLights.size() > 0)
		{
			PointLight* dataPointlight = myPointLights.data();
			memcpy(myLightData.myPointLights, dataPointlight, MAX_POINTLIGHTS * sizeof(PointLight));
			myLightData.amountOfPointLights = (int)myPointLights.size();
		}

		myLightData.directionalLightsDirection.x = myDirectionalLight->myDirectionalLightCamera->GetTransform().GetMatrix().r[2].m128_f32[0];
		myLightData.directionalLightsDirection.y = myDirectionalLight->myDirectionalLightCamera->GetTransform().GetMatrix().r[2].m128_f32[1];
		myLightData.directionalLightsDirection.z = myDirectionalLight->myDirectionalLightCamera->GetTransform().GetMatrix().r[2].m128_f32[2];
		myLightData.directionalLightsIntensity = myDirectionalLight->myIntensity;

		DirectX::XMMATRIX camMat = myDirectionalLight->myDirectionalLightCamera->GetModelMatrix();
		DirectX::XMMATRIX camProjMat = myDirectionalLight->myDirectionalLightCamera->GetProjectionMatrix();
		myLightData.dLightShadowSpaceToWorldSpace = (camMat * camProjMat);

		myLightData.ambientLightIntensity = myDirectionalLight->myAmbientIntensity;
		myLightData.directionalLightsColor = myDirectionalLight->myColor;
	}


	LightBufferData* data = (LightBufferData*)mappedBuffer.pData;
	memcpy(data, &myLightData, sizeof(LightBufferData));
	context->Unmap(myLightBuffer.Get(), 0);

	context->VSSetConstantBuffers((int)BufferSlots::Light, 1, myLightBuffer.GetAddressOf());
	context->PSSetConstantBuffers((int)BufferSlots::Light, 1, myLightBuffer.GetAddressOf());
}


//PointLight
void LightManager::RenderPointLightSpheres()
{
	//Transform transform;

	//for (int i = 0; i < myPointLights.size(); i++)
	//{
	//	transform.SetPosition(myPointLights[i].myPos);
	//	transform.SetScale({ 2.f ,2.f , 2.f });
	//	MeshInstanceRenderData instanceData;
	//	instanceData.renderMode = RenderMode::TRIANGLELIST;
	//	instanceData.vsType = VsType::DefaultPBR;
	//	instanceData.psType = PsType::DebugLightSphere;
	//	instanceData.transform = transform;
	//	instanceData.data = MeshDrawerInstanceData(transform, StaticMeshInstanceData());


	//	GraphicsEngine::GetInstance()->GetForwardRenderer().Render(static_cast<Mesh*>(myLightSphereMesh), instanceData);
	//}
}

void LightManager::RenderPointLightsBounds()
{
#if !_MODELVIEWER
	auto ge = GraphicsEngine::GetInstance();

	//Setup
	Vector3f color = { 0.f, 0.f, 0.f };
	myPointLightsRenderTarget.Clear(color);
	myPointLightsRenderTarget.SetAsTarget(&ge->GetDepthBuffer());
	ge->SetRasterizerState(RasterizerState::FrontfaceCulling);
	ge->SetDepthStencilState(DepthStencilState::ReadOnlyGreater);
	ge->SetBlendState(BlendState::Disabled);

	//Kötta
	Transform transform;
	for (int i = 0; i < myPointLights.size(); i++)
	{
		transform.SetScale({ myPointLights[i].myRange / 10.f ,myPointLights[i].myRange / 10.f ,myPointLights[i].myRange / 10.f });
		transform.SetPosition(myPointLights[i].myPos);

		myLightSphereMesh->Render(transform.GetMatrix(), ShaderDatabase::GetVertexShader(VsType::DefaultPBR), ShaderDatabase::GetPixelShader(PsType::LightBounds), RenderMode::TRIANGLELIST);
	}


	//Reset
	ge->SetRasterizerState(RasterizerState::BackfaceCulling);
	ge->SetDepthStencilState(DepthStencilState::ReadWrite);
#endif
}




void LightManager::RenderDirectionalLight()
{
	GraphicsEngine* ge = GraphicsEngine::GetInstance();
	auto* context = ge->DX().GetContext();

	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(nullptr);

	context->GSSetShader(nullptr, nullptr, 0);

	ge->GetDepthBuffer().CopyToStaging();
	context->PSSetShaderResources((int)PixelShaderTextureSlot::DepthBuffer, 1, ge->GetDepthBuffer().myIntermediateSRV.GetAddressOf());

	ge->SetDepthStencilState(DepthStencilState::Disabled);
	ShaderDatabase::GetVertexShader(VsType::Fullscreen)->PrepareRender();
	ShaderDatabase::GetPixelShader(PsType::DirectionalLight)->PrepareRender();

	Vector3f color = { 0.f, 0.f, 0.f };
	myDirectionalLightRenderTarget.Clear(color);
	myDirectionalLightRenderTarget.SetAsTarget();
	context->Draw(3, 0);

	ID3D11ShaderResourceView* nullsrv = nullptr;
	context->PSSetShaderResources((int)PixelShaderTextureSlot::DepthBuffer, 1, &nullsrv);
}

//Directional Light
void LightManager::SetDirectionalLightsDirection(const Vector3f & aDirection)
{
	myDirectionalLight->myDirection = aDirection;

	SetLightsOnBuffer();
}

void LightManager::SetDirectionalLightsColor(const Vector4f & aColor)
{
	myDirectionalLight->myColor = { aColor.x,aColor.y,aColor.z };
	myDirectionalLight->myIntensity = aColor.w;

	SetLightsOnBuffer();
}

void LightManager::SetDirectionalLightsCameraToPosition(const Vector3f & aPos)
{
	myDirectionalLight->myDirectionalLightCamera->GetTransform().SetPosition(aPos);

	SetLightsOnBuffer();
}

void LightManager::SetAmbientLight(float aIntensity)
{
	myDirectionalLight->myAmbientIntensity = aIntensity;
	SetLightsOnBuffer();
}

void LightManager::BeginShadowRendering()
{


	auto* g = GraphicsEngine::GetInstance();
	myLastUsedCamera = g->GetCamera();
	SetDirectionalLightsCameraToPosition(g->GetCamera()->GetTransform().GetPosition());
	myDirectionalLight->myShadowMapDepthBuffer.Clear();
	g->ChangeCurrentCamera(myDirectionalLight->myDirectionalLightCamera);
	myDirectionalLight->myShadowMapDepthBuffer.SetAsActiveTarget();

	//SetLightsOnBuffer();
}

void LightManager::EndShadowRendering()
{
	auto* ge = GraphicsEngine::GetInstance();
	auto* context = ge->DX().GetContext();

	//g->ResetToViewCamera();
	ge->ChangeCurrentCamera(myLastUsedCamera);
	context->OMSetRenderTargets(1, ge->GetBackBuffer().GetAddressOf(), ge->GetDepthBuffer().GetDepthStencilView());
	context->RSSetViewports(1, &ge->DX().GetViewport());
	myDirectionalLight->myShadowMapDepthBuffer.CopyToStaging();
	context->PSSetShaderResources((int)PixelShaderTextureSlot::ShadowMap, 1, myDirectionalLight->myShadowMapDepthBuffer.myIntermediateSRV.GetAddressOf());

	SetLightsOnBuffer();
}

void LightManager::EndOfRenderPassReset()
{
	ID3D11ShaderResourceView* empty = nullptr;
	GraphicsEngine::GetInstance()->DX().GetContext()->PSSetShaderResources(7, 1, &empty);
}

void LightManager::SetResourcesOnSlot()
{
	auto* context = GraphicsEngine::GetInstance()->DX().GetContext();
	context->PSSetShaderResources((int)PixelShaderTextureSlot::DirectionalLight, 1, myDirectionalLightRenderTarget.SRV.GetAddressOf());
	context->PSSetShaderResources((int)PixelShaderTextureSlot::LightBound, 1, myPointLightsRenderTarget.SRV.GetAddressOf());
}

void LightManager::ClearAllResources()
{
	auto* ge = GraphicsEngine::GetInstance();
	auto* context = ge->DX().GetContext();
	ID3D11ShaderResourceView* nullsrv = nullptr;
	context->PSSetShaderResources((int)PixelShaderTextureSlot::LightBound, 1, &nullsrv);
	context->PSSetShaderResources((int)PixelShaderTextureSlot::DirectionalLight, 1, &nullsrv);
}

