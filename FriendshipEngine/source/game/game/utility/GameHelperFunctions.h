#pragma once
#include <pch.h>
#include <component\TransformComponent.h>

static DirectX::XMMATRIX GetWorldTransform(World* aWorld, Entity aEntity)
{
	auto& transform = aWorld->GetComponent<TransformComponent>(aEntity);
	auto parent = transform.parent;

	if (parent == INVALID_ENTITY)
	{
		return transform.transform.GetMatrix();
	}

	return transform.transform.GetMatrix() * GetWorldTransform(aWorld, parent);
}