#include "pch.h"
#include "VFXManager.h"

#include "VFXDatabase.h"
#include "../GraphicsEngine.h"
#include <assets/AssetDatabase.h>
#include <assets/ShaderDatabase.h>
#include "../model/Mesh.h"
#include "ParticleSystemManager.h"

VFXManager::VFXManager(ParticleSystemManager& aParticleSystemManager)
{
	myParticleSystemManager = &aParticleSystemManager;
	myActiveVFX.resize(VFX_MANAGER_DEFAULT_EFFECTS_SIZE);
	myActiveVFXCount = 0;

	myVfxDatabase = VFXDatabase::GetInstance();
}

VFXManager::~VFXManager() { }

size_t VFXManager::InsertEffect(const vfxid_t& aData)
{
	// skrik till DW när det krashar :)
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

				// Since the VisualEffectData and ParticleEmitterData both have their meshId or particleEmitterIndex on the same
				// memory location, it will result in the activeMeshes[i] to be either one of them depending on the input type
				vfx.activeMeshes[i] = cell.meshId;
				vfx.activeMeshesTime[i] = 0;

				// Skapa once??
				if (cell.effectType.type == VisualEffectType::ParticleEmitter)
				{
					ParticleEmitter emitter;
					emitter.spawnAngle = 90.0f;
					emitter.spawnRadius = 100.0f;
					emitter.spawnRate = 250.0f;
					emitter.lifeSpan = 1.0f;
					emitter.speed = 100.0f;
					emitter.size = 1.0f;
					emitter.sizeVariation = 0.1f;

					emitter.position = cell.transform.GetPosition();
					emitter.velocity = { 0, 10, 0 };
					emitter.color = Vector4f(255.0f / 255.0f, 221.0f / 255.0f, 99.0f / 255.0f, 1);

					vfx.activeMeshes[i] = (int)myParticleSystemManager->InsertEffect(emitter);
				}
			}


			vfx.activeMeshesTime[i] += dt;

			//UPPDATERA TRANSFORMENS POSITION??
			if (cell.startTime + cell.duration <= vfx.time)
			{
				// Ta Bort
				if (cell.effectType.type == VisualEffectType::ParticleEmitter && vfx.activeMeshes[i] >= 0)
				{
					myParticleSystemManager->RemoveEmitter((size_t)vfx.activeMeshes[i]);
				}

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

			const VisualEffect& effect = myVfxDatabase->GetEffect(vfx.visualEffectId);
			const VisualEffectCell& cell = effect.GetCell(i);

			switch (cell.effectType.type)
			{
				case VisualEffectType::VFX:
				{
					const SharedMeshPackage& package = AssetDatabase::GetMesh(vfx.activeMeshes[i]);

					DirectX::XMMATRIX matrix = vfx.worldTransform.GetMatrix();

					for (SharedMesh* mesh : package.meshData)
					{
						VfxMeshInstanceData instanceData;
						instanceData.time = vfx.activeMeshesTime[i];

						forward.Render(
							static_cast<Mesh*>(mesh),
							{
								MeshDrawerInstanceData(Transform(cell.transform.GetMatrix() * matrix), instanceData),
								VsType::DefaultVFX,
								cell.effectType.psType,
								RenderMode::TRIANGLELIST
							},
							BlendState::AlphaBlend
						);
					}
					break;
				}
				case VisualEffectType::ParticleEmitter:
				{
					break;
				}
			}
		}
	}
}
