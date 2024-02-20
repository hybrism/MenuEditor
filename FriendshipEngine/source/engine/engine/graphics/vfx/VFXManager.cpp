#include "pch.h"
#include "VFXManager.h"

#include "VFXDatabase.h"
#include "../GraphicsEngine.h"
#include <assets/AssetDatabase.h>
#include <assets/ShaderDatabase.h>
#include "../model/Mesh.h"

VFXManager::VFXManager(VFXDatabase& aDatabase)
	: myVfxDatabase(&aDatabase)
{ }

VFXManager::~VFXManager() { }

size_t VFXManager::InsertEffect(const vfxid_t& aData)
{
	myActiveVFX[myActiveVFXCount] = VFXData{};
	myActiveVFX[myActiveVFXCount].visualEffectId = aData;
	++myActiveVFXCount;
	return myActiveVFXCount - 1;
}

void VFXManager::Play(const size_t& aVisualEffectId)
{
	auto& vfx = myActiveVFX[aVisualEffectId];
	vfx.Reset();
	vfx.isPlaying = true;
}

void VFXManager::Stop(const size_t& aIndex)
{
	auto& vfx = myActiveVFX[aIndex];
	vfx.isPlaying = false;
	vfx.time = 0;
}

void VFXManager::Update(const float& dt)
{
	for (auto& vfx : myActiveVFX)
	{
		//if (vfx == nullptr) { continue; }
		if (!vfx.isPlaying) { continue; }

		if (vfx.IsComplete())
		{
			vfx.Reset();
			continue;
		}

		const VisualEffect& effect = myVfxDatabase->GetEffect(vfx.visualEffectId);

		for (size_t i = 0; i < vfx.size; ++i)
		{
			const VisualEffectCell& cell = effect.GetCell(i);

			if (!vfx.cellHasStarted[i])
			{
				if (cell.startTime > vfx.time) { continue; }
				vfx.cellHasStarted[i] = true;
				vfx.activeMeshes[i] = (int)cell.meshId;
				vfx.activeMeshesTime[i] = 0;
			}

			vfx.activeMeshesTime[i] += dt;

			//UPPDATERA TRANSFORMENS POSITION??
			if (cell.startTime + cell.duration <= vfx.time)
			{
				vfx.activeMeshes[i] = -1;
				vfx.activeMeshesTime[i] = 0;
			}
		}

		vfx.time += dt;
	}
}

#include <engine/utility/Error.h>
void VFXManager::Render()
{
	auto* ge = GraphicsEngine::GetInstance();
	ForwardRenderer& forward = ge->GetForwardRenderer();
	for (auto& vfx : myActiveVFX)
	{
		if (!vfx.isPlaying) { continue; }

		for (size_t i = 0; i < vfx.size; ++i)
		{
			if (vfx.activeMeshes[i] <= 0) { continue; }

			const SharedMeshPackage& package = AssetDatabase::GetMesh(vfx.activeMeshes[i]);

			DirectX::XMMATRIX matrix = vfx.worldTransform.GetMatrix();

			const VisualEffect& effect = myVfxDatabase->GetEffect(vfx.visualEffectId);
			const VisualEffectCell& cell = effect.GetCell(i);
			
			for (SharedMesh* data : package.meshData)
			{
				MeshInstanceRenderData instanceData;
				instanceData.renderMode = RenderMode::TRIANGLELIST;
				instanceData.vsType = VsType::DefaultVFX;
				instanceData.psType = cell.type;
				instanceData.transform = Transform(cell.transform.GetMatrix() * matrix);
				instanceData.data = VfxMeshInstanceData{ vfx.activeMeshesTime[i] };
				forward.Render(static_cast<Mesh*>(data), instanceData, BlendState::AlphaBlend);
			}
		}
	}
}
