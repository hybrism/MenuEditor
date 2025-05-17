#include "pch.h"
#include "ForwardRenderer.h"

#include <assets/ShaderDatabase.h>
#include <engine/graphics/model/Mesh.h>
#include <engine/graphics/model/SkeletalMesh.h>
#include <engine/graphics/GraphicsEngine.h>
#include <engine/graphics/GBuffer.h>
//#include <engine/graphics/DirectionalLightManager.h>
#include <engine/shaders/VertexShader.h>
#include <engine/shaders/PixelShader.h>
#include <engine/graphics/model/MeshDrawer.h>

#if SHOULD_SORT_ALPHA_LAYERS
#include <algorithm>
#include "../Camera.h"
#endif

ForwardRenderer::ForwardRenderer(MeshDrawer& aMeshDrawer) : myMeshDrawer(aMeshDrawer)
{
	for (size_t i = 0; i < (size_t)BlendState::Count; i++)
	{
		myStaticMeshes[i] = {};
		mySkeletalMeshes[i] = {};
	}
}

ForwardRenderer::~ForwardRenderer()
{
	Clear();
}

void ForwardRenderer::Render(Mesh* aMesh, const MeshInstanceRenderData& aInstanceData, BlendState aBlendState)
{
	myStaticMeshes[(size_t)aBlendState][aMesh].push_back(aInstanceData);
}

void ForwardRenderer::Render(SkeletalMesh* aMesh, const MeshInstanceRenderData& aInstanceData, BlendState aBlendState)
{
	mySkeletalMeshes[(size_t)aBlendState][aMesh].push_back(aInstanceData);
}

void ForwardRenderer::DoRenderPass()
{
	GraphicsEngine* ge = GraphicsEngine::GetInstance();
	{

		ge->SetDepthStencilState(DepthStencilState::ReadOnly);
		ge->SetBlendState(BlendState::AlphaBlend);
		ge->SetRasterizerState(RasterizerState::NoFaceCulling);
		//ge->SetBlendState(BlendState::AdditiveBlend);
	}
	ge->DX().GetContext()->PSSetShaderResources(14, 1, ge->GetDepthBuffer().myIntermediateSRV.GetAddressOf());

	RenderMeshes(true);
	Clear();
	ge->SetRasterizerState(RasterizerState::BackfaceCulling);
	ge->SetDepthStencilState(DepthStencilState::ReadWrite);

	ID3D11ShaderResourceView* nullSRV = nullptr;
	ge->DX().GetContext()->PSSetShaderResources(14, 1, &nullSRV);
}

void ForwardRenderer::Clear()
{
	for (size_t i = 0; i < (size_t)BlendState::Count; i++)
	{
		for (auto& mesh : myStaticMeshes[i])
		{
			mesh.second.clear();
		}
		myStaticMeshes[i].clear();
		mySkeletalMeshes[i].clear();
	}
}

#include "../GraphicsData.h"
void ForwardRenderer::RenderMeshes(bool aUsePixelShader)
{
	GraphicsEngine* ge = GraphicsEngine::GetInstance();


	// TODO: Sort alpha blending meshes by distance to camera doesnt work when using different InstanceData

	for (size_t blendState = 0; blendState < (size_t)BlendState::Count; blendState++)
	{
		ge->SetBlendState((BlendState)blendState);
		for (auto& mesh : myStaticMeshes[blendState])
		{
			const Mesh* staticMesh = mesh.first;
			std::vector<MeshInstanceRenderData>& instances = mesh.second;

#if SHOULD_SORT_ALPHA_LAYERS
			if (blendState == (size_t)BlendState::AlphaBlend && instances.size() > 1)
			{
				Vector3f cameraPosition = ge->GetCamera()->GetTransform().GetPosition();
				std::sort(instances.begin(), instances.end(),
					[cameraPosition](const MeshInstanceRenderData& v0, const MeshInstanceRenderData& v1)
					{
						Vector3f delta0 = v0.data.transform.GetPosition() - cameraPosition;
						Vector3f delta1 = v1.data.transform.GetPosition() - cameraPosition;
						return delta0.Length() > delta1.Length();
					});
			}
#endif

			if (!aUsePixelShader)
			{
				myMeshDrawer.Draw(*staticMesh, instances.data(), instances.size(), ShaderDatabase::GetVertexShader(instances[0].vsType), nullptr);
				continue;
			}
			myMeshDrawer.Draw(*staticMesh, instances.data(), instances.size(), ShaderDatabase::GetVertexShader(instances[0].vsType), ShaderDatabase::GetPixelShader(instances[0].psType));
		}

		// We don't support instancing for skeletal meshes yet
		for (auto& mesh : mySkeletalMeshes[blendState])
		{
			SkeletalMesh* skeletalMesh = mesh.first;
			std::vector<MeshInstanceRenderData>& instances = mesh.second;

			for (size_t i = 0; i < instances.size(); i++)
			{
				SkeletalMeshInstanceData& data = instances[i].data.skeletalMeshData;
				skeletalMesh->SetPose(data.pose);

				const VertexShader* vs = ShaderDatabase::GetVertexShader(instances[i].vsType);
				const PixelShader* ps = nullptr;

				if (aUsePixelShader)
				{
					ps = ShaderDatabase::GetPixelShader(instances[i].psType);
				}

				skeletalMesh->Render(
					instances[i].data.transform.GetMatrix(),
					vs,
					ps,
					instances[i].renderMode
				);
			}
		}
	}
}
