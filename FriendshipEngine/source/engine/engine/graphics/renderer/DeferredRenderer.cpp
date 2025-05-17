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
	myStaticMeshes.clear();
	mySkeletalMeshes.clear();
	myDisregardDepthMeshes.clear();
}

void DeferredRenderer::Render(Mesh* aMesh, MeshInstanceRenderData aInstanceData)
{
	if (aInstanceData.data.type == InstanceDataType::SkeletalMesh)
	{
		aInstanceData.data.staticMeshData.entityData.y = 0;
	}

	AddToRenderContainer(aMesh, aInstanceData, &myStaticMeshes);
}

void DeferredRenderer::Render(SkeletalMesh* aMesh, const MeshInstanceRenderData& aInstanceData, bool aShouldDisregardDepth)
{
	if (aShouldDisregardDepth)
	{
		AddToRenderContainer(aMesh, aInstanceData, &myDisregardDepthMeshes);
		return;
	}

	AddToRenderContainer(aMesh, aInstanceData, &mySkeletalMeshes);
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
	for (auto& [mesh, data] : myStaticMeshes)
	{
		data.size = 0;
	}
	for (auto& [mesh, data] : mySkeletalMeshes)
	{
		data.size = 0;
	}
	for (auto& [mesh, data] : myDisregardDepthMeshes)
	{
		data.size = 0;
	}
}

void DeferredRenderer::AddToRenderContainer(SharedMesh* aMesh, MeshInstanceRenderData aInstanceData, void* aContainer)
{
	StaticData& container = (*(std::unordered_map<SharedMesh*, StaticData>*)aContainer)[aMesh];
	size_t& size = container.size;
	if (size + 1 >= container.data.size())
	{
		container.data.push_back(aInstanceData);
		++size;
		return;
	}

	container.data[size] = aInstanceData;
	++size;
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
	for (auto& [staticMesh, instances ] : myStaticMeshes)
	{
		if (instances.size == 0) { continue; }

		if (!aUsePixelShader)
		{
			myMeshDrawer.Draw(*staticMesh, instances.data.data(), instances.size, ShaderDatabase::GetVertexShader(instances.data[0].vsType), nullptr);
			continue;
		}

		myMeshDrawer.Draw(
			*staticMesh,
			instances.data.data(),
			instances.size,
			ShaderDatabase::GetVertexShader(instances.data[0].vsType),
			ShaderDatabase::GetPixelShader(instances.data[0].psType)
		);
	}

	// We don't support instancing for skeletal meshes yet
	for (auto& [ mesh, instances ] : mySkeletalMeshes)
	{
		SkeletalMesh* skeletalMesh = (SkeletalMesh*)mesh;

		for (size_t i = 0; i < instances.size; i++)
		{
			SkeletalMeshInstanceData& data = instances.data[i].data.skeletalMeshData;
			skeletalMesh->SetPose(data.pose);

			const VertexShader* vs = ShaderDatabase::GetVertexShader(instances.data[i].vsType);
			const PixelShader* ps = nullptr;

			if (aUsePixelShader)
			{
				ps = ShaderDatabase::GetPixelShader(instances.data[i].psType);
			}

			skeletalMesh->Render(
				instances.data[i].data.transform.GetMatrix(),
				vs,
				ps,
				instances.data[i].renderMode
			);
		}
	}

	auto* ge = GraphicsEngine::GetInstance();
	DepthStencilState previousState = ge->GetRenderState().depthStencilState;
	for (auto& [mesh, instances] : myDisregardDepthMeshes)
	{
		SkeletalMesh* skeletalMesh = (SkeletalMesh*)mesh;

		for (size_t i = 0; i < instances.size; i++)
		{
			SkeletalMeshInstanceData& data = instances.data[i].data.skeletalMeshData;
			skeletalMesh->SetPose(data.pose);

			const VertexShader* vs = ShaderDatabase::GetVertexShader(instances.data[i].vsType);
			const PixelShader* ps = nullptr;

			if (aUsePixelShader)
			{
				ps = ShaderDatabase::GetPixelShader(instances.data[i].psType);
			}

			Transform copy = instances.data[i].data.transform;
			copy.Translate(copy.GetForward() * 10.0f);
			ge->SetDepthStencilState(DepthStencilState::Disabled);
			skeletalMesh->Render(
				copy.GetMatrix(),
				vs,
				ps,
				instances.data[i].renderMode
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
