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

void DeferredRenderer::Render(Mesh* aMesh, MeshInstanceRenderData aInstanceData)
{
	if (std::holds_alternative<StaticMeshInstanceData>(aInstanceData.data))
	{
		std::get<StaticMeshInstanceData>(aInstanceData.data).entityData.y = static_cast<unsigned int>(myStaticMeshes[aMesh].size());
	}
	myStaticMeshes[aMesh].push_back(aInstanceData);
}

void DeferredRenderer::Render(SkeletalMesh* aMesh, const MeshInstanceRenderData& aInstanceData, bool aShouldDisregardDepth)
{
	if (aShouldDisregardDepth)
	{
		myDisregardDepthMeshes[aMesh].push_back(aInstanceData);
		return;
	}
	mySkeletalMeshes[aMesh].push_back(aInstanceData);
}

void DeferredRenderer::DoShadowRenderPass()
{
	//auto* lightManager = GraphicsEngine::GetInstance()->GetDirectionalLightManager();
	//lightManager->BeginShadowRendering();
	GraphicsEngine::GetInstance()->SetDepthStencilState(DepthStencilState::ReadWrite);
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
	myDisregardDepthMeshes.clear();
}

void DeferredRenderer::PrepareGBuffer()
{
	GraphicsEngine* ge = GraphicsEngine::GetInstance();
	auto& gBuffer = ge->GetGBuffer();

	gBuffer.ClearTextures();
	gBuffer.SetAsActiveTarget();
	ge->GetDepthBuffer().Clear();

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
			myMeshDrawer.Draw(*staticMesh, instances.data(), instances.size(), ShaderDatabase::GetVertexShader(instances[0].vsType), nullptr);
			continue;
		}
		myMeshDrawer.Draw(*staticMesh, instances.data(), instances.size(), ShaderDatabase::GetVertexShader(instances[0].vsType), ShaderDatabase::GetPixelShader(instances[0].psType));
	}

	// We don't support instancing for skeletal meshes yet
	for (auto& mesh : mySkeletalMeshes)
	{
		SkeletalMesh* skeletalMesh = mesh.first;
		std::vector<MeshInstanceRenderData>& instances = mesh.second;

		for (size_t i = 0; i < instances.size(); i++)
		{
			auto& data = std::get<SkeletalMeshInstanceData>(instances[i].data);
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

	auto* ge = GraphicsEngine::GetInstance();
	DepthStencilState previousState = ge->GetRenderState().depthStencilState;
	for (auto& mesh : myDisregardDepthMeshes)
	{
		SkeletalMesh* skeletalMesh = mesh.first;
		std::vector<MeshInstanceRenderData>& instances = mesh.second;

		for (size_t i = 0; i < instances.size(); i++)
		{
			auto& data = std::get<SkeletalMeshInstanceData>(instances[i].data);
			skeletalMesh->SetPose(data.pose);

			const VertexShader* vs = ShaderDatabase::GetVertexShader(instances[i].vsType);
			const PixelShader* ps = nullptr;

			if (aUsePixelShader)
			{
				ps = ShaderDatabase::GetPixelShader(instances[i].psType);
			}

			Transform copy = data.transform;
			copy.Translate(copy.GetForward() * 10.0f);
			ge->SetDepthStencilState(DepthStencilState::Disabled);
			skeletalMesh->Render(
				copy.GetMatrix(),
				vs,
				ps,
				instances[i].renderMode
			);
		}
	}
	ge->SetDepthStencilState(previousState);
}

void DeferredRenderer::RenderDeferred()
{
	GraphicsEngine* ge = GraphicsEngine::GetInstance();
	auto& gBuffer = ge->GetGBuffer();
	auto* context = ge->DX().GetContext();

	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(nullptr);

	context->GSSetShader(nullptr, nullptr, 0);

	ge->SetDepthStencilState(DepthStencilState::Disabled);

	ShaderDatabase::GetVertexShader(VsType::Fullscreen)->PrepareRender();
	ShaderDatabase::GetPixelShader(PsType::DeferredPBR)->PrepareRender();

	ge->SetBlendState(BlendState::Disabled);
	gBuffer.SetAllAsResources(1);
	context->Draw(3, 0);
	gBuffer.ClearAllResources(1);
}
