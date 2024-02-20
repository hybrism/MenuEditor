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
	delete myDirectionalLight->myShadowMapDepthBuffer;
	delete myDirectionalLight;


	myPointLights.clear();

	delete myBoundingPixelShader;

}

void LightManager::ImportLights()
{
}

void LightManager::LightRender()
{
	//auto ge = GraphicsEngine::GetInstance();

	//ge->SetBlendState(BlendState::AdditiveBlend);
	//RenderPointLightsBounds();
	RenderDirectionalLight();
	//ge->SetBlendState(BlendState::Disabled);

}

void LightManager::Init(const Vector3f& aDirection, const Vector4f& aColor)
{
	auto* g = GraphicsEngine::GetInstance();

	myDirectionalLight = new DirectionalLight(aDirection, { aColor.x,aColor.y,aColor.z }, aColor.w);
	myDirectionalLight->myDirectionalLightCamera = new Camera;
	myDirectionalLight->myDirectionalLightCamera->SetProjectionMatrix(60.2f, (float)g->GetWindowDimensions().x, (float)g->GetWindowDimensions().y, 1.f, 10000.f);
	myDirectionalLight->myDirectionalLightCamera->SetOrtographicProjection(-2500, 2500, -2500, 2500, -7500.f, 5000.f); //-2500, 2500, -2500, 2500, -7500.f, 5000.f
	myDirectionalLight->myDirectionalLightCamera->SetPosition({ 10, 100, 10 });
	myDirectionalLight->myDirectionalLightCamera->SetRotation(aDirection);
	//myDirectionalLight->myShadowMapDepthBuffer = DepthBuffer::CreateNewPointer(g->GetViewportDimensions());  //KOMMER BLI FUCKED I NÄSTA UPÅPDATERING

	//myBoundingPixelShader = new PixelShader;
	myBoundingPixelShader = ShaderDatabase::GetPixelShader(PsType::LightBounds);
	myLightSphereMesh = AssetDatabase::GetMesh((int)PrimitiveMeshID::Sphere).meshData[0];


	{
		//HRESULT result;
		myPointLightsRenderTarget = new RenderTarget;
		*myPointLightsRenderTarget = RenderTarget::Create(GraphicsEngine::GetInstance()->GetBackTexturesize(), DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
		//D3D11_TEXTURE2D_DESC desc = { 0 };
		//desc.Width = (int)GraphicsEngine::GetInstance()->GetBackTexturesize().x;
		//desc.Height = (int)GraphicsEngine::GetInstance()->GetBackTexturesize().y;
		//desc.MipLevels = 1;
		//desc.ArraySize = 1;
		//desc.Format = DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
		//desc.SampleDesc.Count = 1;
		//desc.SampleDesc.Quality = 0;
		//desc.Usage = D3D11_USAGE_DEFAULT;
		//desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		//desc.CPUAccessFlags = 0;
		//desc.MiscFlags = 0;
		//ID3D11Texture2D* texture;
		//result = GraphicsEngine::GetInstance()->GetDevice()->CreateTexture2D(&desc, nullptr, &texture);
		//assert(SUCCEEDED(result));
		//result = GraphicsEngine::GetInstance()->GetDevice()->CreateShaderResourceView(texture, nullptr, &myPointLightsRenderTarget->shaderResourceView);
		//assert(SUCCEEDED(result));
		//result = GraphicsEngine::GetInstance()->GetDevice()->CreateRenderTargetView(texture, nullptr, &myPointLightsRenderTarget->renderTargetView);
		//assert(SUCCEEDED(result));
		//texture->Release();
	}
	{
		//HRESULT result;
		myDirectionalLightRenderTarget = new RenderTarget;
		*myDirectionalLightRenderTarget = RenderTarget::Create(GraphicsEngine::GetInstance()->GetBackTexturesize(), DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
		/*D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = (int)GraphicsEngine::GetInstance()->GetBackTexturesize().x;
		desc.Height = (int)GraphicsEngine::GetInstance()->GetBackTexturesize().y;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		ID3D11Texture2D* texture;
		result = GraphicsEngine::GetInstance()->GetDevice()->CreateTexture2D(&desc, nullptr, &texture);
		assert(SUCCEEDED(result));
		result = GraphicsEngine::GetInstance()->GetDevice()->CreateShaderResourceView(texture, nullptr, &myDirectionalLightRenderTarget->shaderResourceView);
		assert(SUCCEEDED(result));
		result = GraphicsEngine::GetInstance()->GetDevice()->CreateRenderTargetView(texture, nullptr, &myDirectionalLightRenderTarget->renderTargetView);
		assert(SUCCEEDED(result));
		texture->Release();*/
	}

	//PointLight* temp = new PointLight({ 100,20,-20 }, { 255,255,255 }, 150, 255);
	//myPointLights.push_back(temp);

	//temp = new PointLight({ -20,20,0 }, { 255,255,255 }, 2550, 255);
	//myPointLights.push_back(temp);

	//temp = new PointLight({ 20,10,10 }, { 255,255,255 }, 2550, 255);
	//myPointLights.push_back(temp);

	//temp = new PointLight({ 60,30,-100 }, { 255,255,255 }, 2550, 255);
	//myPointLights.push_back(temp);

	//Skapa Light Buffer
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

		//for (int i = 0; i < myPointLights.size(); i++)
		//{
		//	data->myPointLights[i].pointLightsPosition = myPointLights[i].myPos;
		//	data->myPointLights[i].pointLightsIntensity = myPointLights[i].myIntensity;
		//	data->myPointLights[i].pointLightsColor = myPointLights[i].myColor;
		//	data->myPointLights[i].pointLightsRange = myPointLights[i].myRange;
		//}
		data->amountOfPointLights = (int)myPointLights.size();
	}



	data->directionalLightsDirection.x = myDirectionalLight->myDirection.x;
	data->directionalLightsDirection.y = myDirectionalLight->myDirection.y * -1.f;
	data->directionalLightsDirection.z = myDirectionalLight->myDirection.z;
	data->directionalLightsDirection.w = 1.f;

	Matrix4x4f camMat = myDirectionalLight->myDirectionalLightCamera->GetModelMatrix();
	Matrix4x4f camProjMat = myDirectionalLight->myDirectionalLightCamera->GetProjectionMatrix();
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
	auto context = ge->GetContext();


	//Rensa min plats
	ID3D11ShaderResourceView* nullsrv = nullptr;
	context->PSSetShaderResources((int)ShaderTextureSlot::LightBound, 1, &nullsrv);  //to:do Enum
	float color[4] = { 0.f, 0.f,0.f,1.0f };
	context->ClearRenderTargetView(myPointLightsRenderTarget->renderTargetView.Get(), color);

	//Setup
	//context->OMSetRenderTargets(1, myPointLightsRenderTarget->renderTargetView.GetAddressOf(), ge->GetGBuffer().GetDepthBuffer().GetDepthStencilView());
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

	//Kom i åt via editorn
	//ImGui::Image(myPointLightsRenderTarget->shaderResourceView.Get(), { 200,150 });


	//Reset
	ge->SetRasterizerState(RasterizerState::BackfaceCulling);
	ge->SetDepthStencilState(DepthStencilState::ReadWrite);



	//Sätt på min plats  SÄTT PÅ g-buffern
	ID3D11RenderTargetView* nullrtv = nullptr;
	ge->GetContext()->OMSetRenderTargets(1, &nullrtv, nullptr);
	context->PSSetShaderResources((int)ShaderTextureSlot::LightBound, 1, myPointLightsRenderTarget->shaderResourceView.GetAddressOf());

	//ImGui::Image(myPointLightsRenderTarget->shaderResourceView.Get(), {200,150});
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

	ID3D11ShaderResourceView* nullsrv = nullptr;
	context->PSSetShaderResources((int)ShaderTextureSlot::LightBound, 1, &nullsrv); // TODO ENUM
	float color[4] = { 0.f, 0.f,0.f,1.0f };
	context->ClearRenderTargetView(myDirectionalLightRenderTarget->renderTargetView.Get(), color);
	//context->OMSetRenderTargets(1, myDirectionalLightRenderTarget->renderTargetView.GetAddressOf(), ge->GetGBuffer().GetDepthBuffer().GetDepthStencilView());


	context->Draw(3, 0);


	//Sätt på min plats G_BUFFER
	ID3D11RenderTargetView* nullrtv = nullptr;
	ge->GetContext()->OMSetRenderTargets(1, &nullrtv, nullptr);
	context->PSSetShaderResources((int)ShaderTextureSlot::LightBound, 1, myDirectionalLightRenderTarget->shaderResourceView.GetAddressOf());

	//ImGui::Image(myDirectionalLightRenderTarget->shaderResourceView.Get(), { 200,150 });
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
	myDirectionalLight->myDirectionalLightCamera->SetPosition(aPos);

	SetLightsOnBuffer();
}

void LightManager::BeginShadowRendering()
{
	auto* g = GraphicsEngine::GetInstance();
	auto* context = g->GetContext();

	context->ClearDepthStencilView(myDirectionalLight->myShadowMapDepthBuffer->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	g->ChangeCurrentCamera(myDirectionalLight->myDirectionalLightCamera);
	myDirectionalLight->myShadowMapDepthBuffer->SetAsActiveTarget();

	SetLightsOnBuffer();
}

void LightManager::EndShadowRendering()
{
	auto* g = GraphicsEngine::GetInstance();
	auto* context = g->GetContext();

	g->ResetToPrimaryCamera();
	context->OMSetRenderTargets(1, g->GetBackBuffer().GetAddressOf(), g->GetDepthBuffer().GetDepthStencilView());
	context->RSSetViewports(1, &g->GetViewPort());
	context->PSSetShaderResources(7, 1, myDirectionalLight->myShadowMapDepthBuffer->mySRV.GetAddressOf());  //ENUM

	SetLightsOnBuffer();
}

void LightManager::EndOfRenderPassReset()
{
	ID3D11ShaderResourceView* empty = nullptr;
	GraphicsEngine::GetInstance()->GetContext()->PSSetShaderResources(7, 1, &empty);  //ENUM
}

