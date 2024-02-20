#pragma once

namespace fbxsdk
{
	class FbxNode;
	class FbxScene;
	class FbxAMatrix;
	class FbxVector4;
	template <class Type>
	class FbxLayerElementTemplate;
};

class SharedFactoryFunctions
{
public:
	static bool GetSkeleton(
		fbxsdk::FbxNode* aRootNode,
		int someParentIndex,
		Skeleton& outSkeleton
	);

	static DirectX::XMMATRIX FBXMatrixToDXMatrix(fbxsdk::FbxAMatrix& aMatrix);
};

