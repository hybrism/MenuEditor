#pragma once
//#include <component\TransformComponent.h>

//static DirectX::XMMATRIX GetWorldTransform(World* aWorld, Entity aEntity)
//{
//	auto& globalTransform = aWorld->GetComponent<TransformComponent>(aEntity);
//	auto parent = globalTransform.parent;
//
//	if (parent == INVALID_ENTITY)
//	{
//		return globalTransform.globalTransform.GetMatrix();
//	}
//
//	return globalTransform.globalTransform.GetMatrix() * GetWorldTransform(aWorld, parent);
//}
struct TransformComponent;

class GameHelperFunctions
{
public:
	static DirectX::XMMATRIX GetWorldTransform(World* aWorld, Entity aEntity);
	//friend struct TransformComponent;
};


//extern Transform SetWorldTransform(World* aWorld, Entity aEntity);




