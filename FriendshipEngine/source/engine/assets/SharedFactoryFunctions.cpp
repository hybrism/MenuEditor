#include "pch.h"

DirectX::XMMATRIX SharedFactoryFunctions::FBXMatrixToDXMatrix(fbxsdk::FbxAMatrix& aMatrix)
{
	float matrixArray[16]
	{
		(float)aMatrix.GetColumn(0)[0], (float)aMatrix.GetColumn(0)[1], (float)aMatrix.GetColumn(0)[2], (float)aMatrix.GetColumn(0)[3],
		(float)aMatrix.GetColumn(1)[0], (float)aMatrix.GetColumn(1)[1], (float)aMatrix.GetColumn(1)[2], (float)aMatrix.GetColumn(1)[3],
		(float)aMatrix.GetColumn(2)[0], (float)aMatrix.GetColumn(2)[1], (float)aMatrix.GetColumn(2)[2], (float)aMatrix.GetColumn(2)[3],
		(float)aMatrix.GetColumn(3)[0], (float)aMatrix.GetColumn(3)[1], (float)aMatrix.GetColumn(3)[2], (float)aMatrix.GetColumn(3)[3]
	};
	return DirectX::XMMATRIX(matrixArray);
}

bool SharedFactoryFunctions::GetSkeleton(
	FbxNode* aRootNode,
	int someParentIndex,
	Skeleton& outSkeleton
)
{
	int index = someParentIndex;

	FbxNodeAttribute* nodeAttribute = aRootNode->GetNodeAttribute();
	if (nodeAttribute)
	{
		if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			// We're a bone, process as normal
			//FBXSkeleton::FBXBone bone;
			Bone bone;
			bone.fullName = aRootNode->GetName();
			bone.parentId = someParentIndex;

			bone.name = bone.fullName;
			if (const size_t pos = bone.name.find_last_of(':'); pos != std::string::npos)
			{
				bone.name = bone.name.substr(pos + 1);
			}
			outSkeleton.bones.push_back(bone);

			index = static_cast<int>(outSkeleton.bones.size() - 1ULL);
			bone.id = index;

			if (outSkeleton.boneNameToIndexMap.find(bone.name) != outSkeleton.boneNameToIndexMap.end())
			{
				std::throw_with_nested(std::runtime_error("Found more than one bone with the same name [" + bone.fullName + "]!"));
			}

			outSkeleton.boneNameToIndexMap.insert({ bone.name, index });

			if (someParentIndex >= 0)
			{
				outSkeleton.bones[someParentIndex].childrenIds.push_back(index);
			}
		}
	}

	for (int i = 0; i < aRootNode->GetChildCount(); i++)
	{
		GetSkeleton(aRootNode->GetChild(i), index, outSkeleton);
	}

	return !outSkeleton.bones.empty();
}
