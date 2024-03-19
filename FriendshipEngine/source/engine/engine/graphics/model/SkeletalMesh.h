#pragma once
#include "SharedMesh.h"
#include "../animation/Skeleton.h"

#ifndef _RELEASE
#include <vector>
#include "../../debug/DebugLine.h"
#endif

class SkeletalMesh : public SharedMesh
{
public:
	SkeletalMesh();
	~SkeletalMesh() override;
	bool Initialize(
		AnimatedVertex aVertices[],
		const size_t& aVertexCount,
		unsigned int aIndices[],
		const size_t& aIndexCount,
		const bool& aUseDynamicVertexBuffer = false
	);

	void Render(const DirectX::XMMATRIX& aTransform, const VertexShader* aVS, const PixelShader* aPS, const RenderMode& aRenderMode = RenderMode::TRIANGLELIST) const override;
	void RenderSkeleton(const DirectX::XMMATRIX& aTransform);
	void SetPose(const Pose& aLocalSpacePose);
	const Pose& GetPose() const { return myPose; }
	void ResetPose();

	void SetSkeleton(const Skeleton& aSkeleton);
	const Skeleton& GetSkeleton() const;
private:
#ifndef _RELEASE
	std::vector<DebugLine> myDebugLines;
#endif
	Skeleton mySkeleton;
	Pose myPose = {};
};