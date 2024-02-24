#include "pch.h"
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
	auto* context = GraphicsEngine::GetInstance()->GetContext();
	context->OMGetRenderTargets(1, &RTV, &DSV);

	ClearAllResources();
	RenderDirectionalLight();
	RenderPointLightsBounds();


	context->OMSetRenderTargets(1, &RTV, DSV);
}

void LightManager::Init(const Vector3f& aDirection, const Vector4f& aColor)
{
	myPointLights = AssetDatabase::GetPointLight();



	auto* g = GraphicsEngine::GetInstance();
	aDirection;
	aColor;

	//If intensity is less than 0, then there is no Directional Light.
	if (AssetDatabase::GetDirectionalLight().myIntensity < 0)
	{
		std::cout << "[ERROR] [LightManager.cpp] There was no Directional Light to import, creating a hardcoded one ;) \n";
		myDirectionalLight = new DirectionalLight(aDirection, { aColor.x, aColor.y, aColor.z }, aColor.w);
	}
	else
	{
		myDirectionalLight = new DirectionalLight(AssetDatabase::GetDirectionalLight().myDirection, AssetDatabase::GetDirectionalLight().myColor, AssetDatabase::GetDirectionalLight().myIntensity);
	}
	myDirectionalLight->myDirectionalLightCamera = new Camera;
	myDirectionalLight->myDirectionalLightCamera->SetProjectionMatrix(60.2f, (float)g->GetWindowDimensions().x, (float)g->GetWindowDimensions().y, 100.f, 100000.f);
	myDirectionalLight->myDirectionalLightCamera->SetOrtographicProjection(-2500, 2500, -2500, 2500, -7500.f, 5000.f);
	myDirectionalLight->myDirectionalLightCamera->GetTransform().SetPosition({ 0,0,0 });
	myDirectionalLight->myDirectionalLightCamera->GetTransform().SetEulerAngles({ 90,0,0 });
	myDirectionalLight->myShadowMapDepthBuffer = DepthBuffer::Create(g->GetViewportDimensions());

	myBoundingPixelShader = ShaderDatabase::GetPixelShader(PsType::LightBounds);
	myLightSphereMesh = AssetDatabase::GetMesh((int)PrimitiveMeshID::Sphere).meshData[0];

	myPointLightsRenderTarget = RenderTarget::Create(GraphicsEngine::GetInstance()->GetViewportDimensions(), DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
	myDirectionalLightRenderTarget = RenderTarget::Create(GraphicsEngine::GetInstance()->GetViewportDimensions(), DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);

	HRESULT result;
	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDescription.ByteWidth = sizeof(LightBufferData);
	result = g->GetDevice()->CreateBuffer(&bufferDescription, nullptr, &myLightBuffer);

	if (FAILED(result))
	{
		std::cout << (" \n [ERROR] [LightManager.cpp]Could not create Light Buffer  \n");
	}


	SetLightsOnBuffer();
}

void LightManager::SetLightsOnBuffer()
{
	auto* g = GraphicsEngine::GetInstance();
	auto* context = g->GetContext();
	D3D11_MAPPED_SUBRESOURCE mappedBuffer{};


	auto result = context->Map(myLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	if (FAILED(result))
	{
		std::cout << ("[ERROR] [LightManager.cpp] Could not LightBuffer \n");
	}
	LightBufferData* data = (LightBufferData*)mappedBuffer.pData;


	if (myPointLights.size() > 0)
	{
		PointLight* dataPointlight = myPointLights.data();
		memcpy(data->myPointLights, dataPointlight, MAX_POINTLIGHTS * sizeof(PointLight));
		data->amountOfPointLights = (int)myPointLights.size();
	}



	data->directionalLightsDirection.x = myDirectionalLight->myDirection.x;
	data->directionalLightsDirection.y = myDirectionalLight->myDirection.y * -1.f;
	data->directionalLightsDirection.z = myDirectionalLight->myDirection.z;
	data->directionalLightsDirection.w = 1.f;

	DirectX::XMMATRIX camMat = myDirectionalLight->myDirectionalLightCamera->GetModelMatrix();
	DirectX::XMMATRIX camProjMat = myDirectionalLight->myDirectionalLightCamera->GetProjectionMatrix();
	data->dLightShadowSpaceToWorldSpace = (camMat * camProjMat);

	data->directionalLightsIntensity = myDirectionalLight->myIntensity;
	data->directionalLightsColor = myDirectionalLight->myColor;



	context->Unmap(myLightBuffer.Get(), 0);

	context->VSSetConstantBuffers((int)BufferSlots::Light, 1, myLightBuffer.GetAddressOf());
	context->PSSetConstantBuffers((int)BufferSlots::Light, 1, myLightBuffer.GetAddressOf());
}


//PointLight
void LightManager::RenderPointLightSpheres()
{
#ifdef _DEBUG
	Transform transform;

	for (int i = 0; i < myPointLights.size(); i++)
	{
		transform.SetPosition(myPointLights[i].myPos);

		MeshInstanceRenderData instanceData;
		instanceData.renderMode = RenderMode::TRIANGLELIST;
		instanceData.vsType = VsType::DefaultPBR;
		instanceData.psType = PsType::DefaultPBR;
		instanceData.transform = transform;
		instanceData.data = StaticMeshInstanceData{ transform };

		GraphicsEngine::GetInstance()->GetForwardRenderer().Render(static_cast<Mesh*>(myLightSphereMesh), instanceData, BlendState::Disabled);
	}
#endif
}


void LightManager::RenderPointLightsBounds()
{
	auto ge = GraphicsEngine::GetInstance();

	//Setup
	Vector4f color = { 0.f, 0.f, 0.f, 1.0f };
	myPointLightsRenderTarget.Clear(color);
	myPointLightsRenderTarget.SetAsTarget(&ge->GetDepthBuffer());
	ge->SetRasterizerState(RasterizerState::FrontfaceCulling);
	ge->SetDepthStencilState(DepthStencilState::ReadOnlyGreater);


	//Kötta
	Transform transform;
	for (int i = 0; i < myPointLights.size(); i++)
	{
		transform.SetScale({ myPointLights[i].myRange / 10.f ,myPointLights[i].myRange / 10.f ,myPointLights[i].myRange / 10.f });
		transform.SetPosition(myPointLights[i].myPos);

		myLightSphereMesh->Render(transform.GetMatrix(), ShaderDatabase::GetVertexShader(VsType::DefaultPBR), myBoundingPixelShader);
	}


	//Reset
	ge->SetRasterizerState(RasterizerState::BackfaceCulling);
	ge->SetDepthStencilState(DepthStencilState::ReadWrite);
}



void LightManager::RenderDirectionalLight()
{
	GraphicsEngine* ge = GraphicsEngine::GetInstance();
	auto* context = ge->GetContext();

	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(nullptr);

	context->GSSetShader(nullptr, nullptr, 0);

	ge->SetDepthStencilState(DepthStencilState::Disabled);
	ShaderDatabase::GetVertexShader(VsType::Fullscreen)->PrepareRender();
	ShaderDatabase::GetPixelShader(PsType::DirectionalLight)->PrepareRender();

	Vector4f color = { 0.f, 0.f, 0.f, 1.0f };
	myDirectionalLightRenderTarget.Clear(color);
	myDirectionalLightRenderTarget.SetAsTarget();
	context->Draw(3, 0);
}

//Directional Light
void LightManager::SetDirectionalLightsDirection(const Vector3f& aDirection)
{
	myDirectionalLight->myDirection = aDirection;

	SetLightsOnBuffer();
}

void LightManager::SetDirectionalLightsColor(const Vector4f& aColor)
{
	myDirectionalLight->myColor = { aColor.x,aColor.y,aColor.z };
	myDirectionalLight->myIntensity = aColor.w;

	SetLightsOnBuffer();
}

void LightManager::SetDirectionalLightsCameraToPosition(const Vector3f& aPos)
{
	myDirectionalLight->myDirectionalLightCamera->GetTransform().SetPosition(aPos);

	SetLightsOnBuffer();
}

void LightManager::BeginShadowRendering()
{
	auto* g = GraphicsEngine::GetInstance();
	myLastUsedCamera = g->GetCamera();
	//SetDirectionalLightsCameraToPosition(g->GetCamera()->GetTransform().GetPosition());
	myDirectionalLight->myShadowMapDepthBuffer.Clear();
	g->ChangeCurrentCamera(myDirectionalLight->myDirectionalLightCamera);
	myDirectionalLight->myShadowMapDepthBuffer.SetAsActiveTarget();

	//SetLightsOnBuffer();
}

void LightManager::EndShadowRendering()
{
	auto* g = GraphicsEngine::GetInstance();
	auto* context = g->GetContext();

	//g->ResetToViewCamera();
	g->ChangeCurrentCamera(myLastUsedCamera);
	context->OMSetRenderTargets(1, g->GetBackBuffer().GetAddressOf(), g->GetDepthBuffer().GetDepthStencilView());
	context->RSSetViewports(1, &g->GetViewPort());
	context->PSSetShaderResources((int)ShaderTextureSlot::ShadowMap, 1, myDirectionalLight->myShadowMapDepthBuffer.mySRV.GetAddressOf());

	SetLightsOnBuffer();
}

void LightManager::EndOfRenderPassReset()
{
	ID3D11ShaderResourceView* empty = nullptr;
	GraphicsEngine::GetInstance()->GetContext()->PSSetShaderResources(7, 1, &empty);
}

void LightManager::SetResourcesOnSlot()
{
	auto* context = GraphicsEngine::GetInstance()->GetContext();
	context->PSSetShaderResources((int)ShaderTextureSlot::DirectionalLight, 1, myDirectionalLightRenderTarget.SRV.GetAddressOf());
	context->PSSetShaderResources((int)ShaderTextureSlot::LightBound, 1, myPointLightsRenderTarget.SRV.GetAddressOf());
}

void LightManager::ClearAllResources()
{
	auto* ge = GraphicsEngine::GetInstance();
	ID3D11ShaderResourceView* nullsrv = nullptr;
	ge->GetContext()->PSSetShaderResources((int)ShaderTextureSlot::LightBound, 1, &nullsrv);
	ge->GetContext()->PSSetShaderResources((int)ShaderTextureSlot::DirectionalLight, 1, &nullsrv);
}

