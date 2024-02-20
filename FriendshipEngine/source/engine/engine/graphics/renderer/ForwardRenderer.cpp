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
#if SHOULD_SORT_ALPHA_LAYERS
	if (aBlendState == BlendState::AlphaBlend)
	{
		myAlphaBlendingInstanceDataContainer.push_back(&myStaticMeshes[(size_t)aBlendState][aMesh].back());
	}
#endif
}

void ForwardRenderer::Render(SkeletalMesh* aMesh, const MeshInstanceRenderData& aInstanceData, BlendState aBlendState)
{
	mySkeletalMeshes[(size_t)aBlendState][aMesh].push_back(aInstanceData);
#if SHOULD_SORT_ALPHA_LAYERS
	if (aBlendState == BlendState::AlphaBlend)
	{
		myAlphaBlendingInstanceDataContainer.push_back(&mySkeletalMeshes[(size_t)aBlendState][aMesh].back());
	}
#endif
}

void ForwardRenderer::DoShadowRenderPass()
{
	//auto* lightManager = GraphicsEngine::GetInstance()->GetDirectionalLightManager();
	RenderMeshes(false);
	//lightManager->EndShadowRendering(); // TODO: OBS: ORDNINGEN ?R DEPENDANT P? ATT DEN K?R EFTER DEFERRED: FIXA DETTA SEN PLS
}

void ForwardRenderer::DoRenderPass()
{
	{
		GraphicsEngine* ge = GraphicsEngine::GetInstance();

		//ge->SetDepthStencilState(DepthStencilState::Disabled);
		ge->SetBlendState(BlendState::AlphaBlend);
	
		//ge->SetBlendState(BlendState::AdditiveBlend);
	}

	RenderMeshes(true);
	Clear();
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
#if SHOULD_SORT_ALPHA_LAYERS
	myAlphaBlendingInstanceDataContainer.clear();
#endif
}

#include "../GraphicsData.h"
void ForwardRenderer::RenderMeshes(bool aUsePixelShader)
{
	GraphicsEngine* ge = GraphicsEngine::GetInstance();


	// TODO: Sort alpha blending meshes by distance to camera doesnt work when using different InstanceData

//#if SHOULD_SORT_ALPHA_LAYERS
//	Vector3f cameraPosition = ge->GetCamera()->GetPosition();
//
//	std::sort(myAlphaBlendingInstanceDataContainer.begin(), myAlphaBlendingInstanceDataContainer.end(),
//		[cameraPosition](const MeshInstanceRenderData* v0, const MeshInstanceRenderData* v1)
//		{
//			return (std::get<StaticMeshInstanceData>(v0->data).transform.GetPosition() - cameraPosition).LengthSqr() > (std::get<StaticMeshInstanceData>(v1->data).transform.GetPosition() - cameraPosition).LengthSqr();
//		});
//#endif

	for (size_t blendState = 0; blendState < (size_t)BlendState::Count; blendState++)
	{
		ge->SetBlendState((BlendState)blendState);
		for (auto& mesh : myStaticMeshes[blendState])
		{
			const Mesh* staticMesh = mesh.first;
			std::vector<MeshInstanceRenderData>& instances = mesh.second;

			for (size_t i = 0; i < instances.size(); i++)
			{
				{
					auto* instance = GraphicsEngine::GetInstance();
					auto* context = instance->GetContext();
					auto objectBuffer = instance->GetObjectBuffer();

					D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
					context->Map(objectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

					ObjectBufferData* data = (ObjectBufferData*)mappedBuffer.pData;
					data->objectTransform = instances[i].transform.GetMatrix();

					context->Unmap(objectBuffer.Get(), 0);
					context->VSSetConstantBuffers((UINT)BufferSlots::Object, 1, objectBuffer.GetAddressOf());
					context->PSSetConstantBuffers((UINT)BufferSlots::Object, 1, objectBuffer.GetAddressOf());
				}

				if (!aUsePixelShader)
				{
					myMeshDrawer.Draw(*staticMesh, instances[i], ShaderDatabase::GetVertexShader(instances[i].vsType), nullptr);
					continue;
				}
				myMeshDrawer.Draw(*staticMesh, instances[i], ShaderDatabase::GetVertexShader(instances[i].vsType), ShaderDatabase::GetPixelShader(instances[i].psType));
			}
		}

		// We don't support instancing for skeletal meshes yet
		for (auto& mesh : mySkeletalMeshes[blendState])
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
}
