#include <pch.h>
#include "GameHelperFunctions.h"
#include "../component/TransformComponent.h"

//static Transform GameHelperFunctions::GetWorldTransform(World* aWorld, Entity aEntity);
DirectX::XMMATRIX GameHelperFunctions::GetWorldTransform(World* aWorld, Entity aEntity)
{
	auto& transformComponent = aWorld->GetComponent<TransformComponent>(aEntity);
	auto parentID = transformComponent.parent;
	if (parentID == INVALID_ENTITY)
	{
		return transformComponent.transform.GetMatrix();
	}	
	return transformComponent.transform.GetMatrix() * GetWorldTransform(aWorld, parentID);
}

//Transform GameHelperFunctions::CalculateTransform(World* aWorld, Entity aEntity)
//{
//	auto& transformComponent = aWorld->GetComponent<TransformComponent>(aEntity);
//	auto parentID = transformComponent.parent;
//	if (parentID == INVALID_ENTITY)
//	{
//		transformComponent.localTransform = transformComponent.worldTransform;
//		return transformComponent.worldTransform;
//	}
//	auto& transformComponentParent = aWorld->GetComponent<TransformComponent>(parentID);
//	Transform dirtyTransform;
//	dirtyTransform.SetMatrix(
//		transformComponent.worldTransform.GetMatrix() 
//		* 
//		CalculateTransform(aWorld, parentID).GetMatrix());
//	
//	return transformComponent.worldTransform;
//}
