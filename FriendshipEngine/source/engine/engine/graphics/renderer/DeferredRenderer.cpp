#include "pch.h"
#include "DeferredRenderer.h"

#include <assets/ShaderDatabase.h>
#include <engine/graphics/model/Mesh.h>
#include <engine/graphics/model/SkeletalMesh.h>
#include <engine/graphics/GraphicsEngine.h>
#include <engine/graphics/GBuffer.h>
//#include <engine/graphics/DirectionalLightManager.h>
#include <engine/shaders/VertexShader.h>
#include <engine/shaders/PixelShader.h>
#include <engine/graphics/model/MeshDrawer.h>

DeferredRenderer::DeferredRenderer(MeshDrawer& aMeshDrawer) : myMeshDrawer(aMeshDrawer)
{
}

DeferredRenderer::~DeferredRenderer()
{
	Clear();
}

void DeferredRenderer::Render(Mesh* aMesh, const MeshInstanceRenderData& aInstanceData)
{
	myStaticMeshes[aMesh].push_back(aInstanceData);
}

void DeferredRenderer::Render(SkeletalMesh* aMesh, const MeshInstanceRenderData& aInstanceData)
{
	mySkeletalMeshes[aMesh].push_back(aInstanceData);
}

void DeferredRenderer::DoShadowRenderPass()
{
	//auto* lightManager = GraphicsEngine::GetInstance()->GetDirectionalLightManager();
	//lightManager->BeginShadowRendering();
	RenderMeshes(false);
}

void DeferredRenderer::DoGBufferPass()
{
	PrepareGBuffer();
	RenderMeshes(true);
}

void DeferredRenderer::DoFinalPass()
{
	RenderDeferred();
	Clear();
}

void DeferredRenderer::Clear()
{
	for (auto& mesh : myStaticMeshes)
	{
		mesh.second.clear();
	}
	myStaticMeshes.clear();
	mySkeletalMeshes.clear();
}

void DeferredRenderer::PrepareGBuffer()
{
	GraphicsEngine* ge = GraphicsEngine::GetInstance();
	auto* context = ge->GetContext();
	auto& gBuffer = ge->GetGBuffer();

	gBuffer.ClearTextures();
	gBuffer.SetAsActiveTarget();
	context->ClearDepthStencilView(ge->GetDepthBuffer().GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ge->SetBlendState(BlendState::Disabled);
	ge->SetDepthStencilState(DepthStencilState::ReadWrite);

	// f?r ljus
	//renderState.depthStencilState = DepthStencilState::ReadOnlyGreater;
	//renderState.rasterizerState = RasterizerState::FrontfaceCulling;
}

void DeferredRenderer::RenderMeshes(bool aUsePixelShader)
{
	for (auto& mesh : myStaticMeshes)
	{
		const Mesh* staticMesh = mesh.first;
		std::vector<MeshInstanceRenderData>& instances = mesh.second;

		if (!aUsePixelShader)
		{
			myMeshDrawer.Draw(*staticMesh, instances.data(), instances.size(), staticMesh->GetVertexShader(), nullptr);
			continue;
		}
		myMeshDrawer.Draw(*staticMesh, instances.data(), instances.size(), staticMesh->GetVertexShader(), ShaderDatabase::GetPixelShader(instances[0].psType));
	}

	// We don't support instancing for skeletal meshes yet
	for (auto& mesh : mySkeletalMeshes)
	{
		SkeletalMesh* skeletalMesh = mesh.first;
		std::vector<MeshInstanceRenderData>& instances = mesh.second;

		for (size_t i = 0; i < instances.size(); i++)
		{
			auto data = std::get<SkeletalMeshInstanceData>(instances[i].data);
			skeletalMesh->SetPose(data.pose);

			const VertexShader* vs = ShaderDatabase::GetVertexShader(instances[i].vsType);
			const PixelShader* ps = nullptr;

			if (aUsePixelShader)
			{
				ps = ShaderDatabase::GetPixelShader(instances[i].psType);
			}

			skeletalMesh->Render(
				data.transform.GetMatrix(), 
				vs,
				ps,
				instances[i].renderMode
			);
		}
	}
}

void DeferredRenderer::RenderDeferred()
{
	GraphicsEngine* ge = GraphicsEngine::GetInstance();
	auto& gBuffer = ge->GetGBuffer();
	auto* context = ge->GetContext();

	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(nullptr);

	context->GSSetShader(nullptr, nullptr, 0);

	ge->SetDepthStencilState(DepthStencilState::Disabled);

	ShaderDatabase::GetVertexShader(VsType::Fullscreen)->PrepareRender();
	ShaderDatabase::GetPixelShader(PsType::DeferredPBR)->PrepareRender();

	ge->SetBlendState(BlendState::AlphaBlend);
	gBuffer.SetAllAsResources(1);
	context->Draw(3, 0);
	gBuffer.ClearAllResources(1);
}
