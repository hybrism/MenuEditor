#include "pch.h"
#include "SkyBox.h"
#include <assets\AssetDatabase.h>
#include "..\model\SharedMesh.h"
#include "..\model\Mesh.h"
#include "..\renderer\ForwardRenderer.h"
#include "..\GraphicsEngine.h"
#include <directxtk/DDSTextureLoader.h>
#include "..\GraphicsEngine.h"
#include <graphics/Camera.h>
#include <Paths.h>

SkyBox::SkyBox()
{
}

SkyBox::~SkyBox()
{
}

void SkyBox::Init()
{
	auto device = GraphicsEngine::GetInstance()->DX().GetDevice();
	auto context = GraphicsEngine::GetInstance()->DX().GetContext();
	std::string path = RELATIVE_ASSET_PATH;
	path = path + "Skybox/t_cubemap_P7_c.dds";
	std::wstring strung(path.begin(), path.end());

	mySkyBoxMesh = AssetDatabase::GetMesh((int)PrimitiveMeshID::Cube).meshData[0];

	HRESULT result = DirectX::CreateDDSTextureFromFile(device.Get(), strung.c_str(), nullptr, &myCubemap);
	assert(!FAILED(result));

	ID3D11Resource* theResource;
	myCubemap->GetResource(&theResource);
	ID3D11Texture2D* cubeTexture = reinterpret_cast<ID3D11Texture2D*>(theResource);
	D3D11_TEXTURE2D_DESC cubeDescription;
	cubeTexture->GetDesc(&cubeDescription);

	myNumMips = cubeDescription.MipLevels;

	context->PSSetShaderResources(13, 1, myCubemap.GetAddressOf());
}


void SkyBox::Update()
{
}

#include <assets/ShaderDatabase.h>
void SkyBox::Render()
{
	auto* ge = GraphicsEngine::GetInstance();
	auto cam = ge->GetCamera();
	Vector3f pos = cam->GetTransform().GetPosition();
	Transform transform;
	transform.SetPosition(pos);
	transform.SetScale({ 20000.f , 20000.f, 20000.f });
	//MeshInstanceRenderData instanceData;
	//instanceData.renderMode = RenderMode::TRIANGLELIST;
	//instanceData.vsType = VsType::DefaultPBRInstanced;
	//instanceData.psType = PsType::DebugLightSphere;
	////instanceData.transform = transform;
	
	auto context = GraphicsEngine::GetInstance()->DX().GetContext();
	context->PSSetShaderResources(13, 1, myCubemap.GetAddressOf());

	ge->SetDepthStencilState(DepthStencilState::Disabled);
	ge->SetBlendState(BlendState::Disabled);
	ge->SetRasterizerState(RasterizerState::NoFaceCulling);
	mySkyBoxMesh->Render(
		transform.GetMatrix(),
		ShaderDatabase::GetVertexShader(VsType::DefaultPBR),
		ShaderDatabase::GetPixelShader(PsType::SkyBox),
		RenderMode::TRIANGLELIST
	);
	ge->SetDepthStencilState(DepthStencilState::ReadWrite);
	ge->SetRasterizerState(RasterizerState::BackfaceCulling);


	//GraphicsEngine::GetInstance()->GetForwardRenderer().Render(
	//	static_cast<Mesh*>(mySkyBoxMesh),
	//	{
	//		instanceData,
	//		VsType::DefaultPBRInstanced,
	//		PsType::SkyBox,
	//		RenderMode::TRIANGLELIST,
	//		{}
	//	}
	//);
}
