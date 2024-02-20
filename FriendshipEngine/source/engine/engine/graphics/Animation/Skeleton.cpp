#include "pch.h"
#include "Skeleton.h"

void Skeleton::ConvertPoseToModelSpace(const Pose& in, Pose& out) const
{
	ConvertPoseToModelSpace(in, out, 0, {});
	out.count = in.count;
}

void Skeleton::ConvertPoseToModelSpace(const Pose& in, Pose& out, unsigned aBoneIdx, const Transform& aParentTransform) const
{
	const Bone& joint = bones[aBoneIdx];

	out.jointTransforms[aBoneIdx] = in.jointTransforms[aBoneIdx].GetMatrix() * aParentTransform.GetMatrix();

	for (size_t c = 0; c < joint.childrenIds.size(); c++)
	{
		ConvertPoseToModelSpace(in, out, joint.childrenIds[c], out.jointTransforms[aBoneIdx]);
	}
}

void Skeleton::ApplyBindPoseInverse(const Pose& in, Transform* out) const
{
	for (size_t i = 0; i < bones.size(); i++)
	{
		const Bone& joint = bones[i];
		out[i] = joint.inverseBindPose * in.jointTransforms[i].GetMatrix();
	}
}

Transform Skeleton::GetBoneModelSpaceTransform(const Pose& in, const std::string& aBoneName) const
{
	return GetBoneModelSpaceTransform(in, boneNameToIndexMap.at(aBoneName));
}

Transform Skeleton::GetBoneModelSpaceTransform(const Pose& in, const int aBoneIndex) const
{
	const Bone* currentJoint = &bones[aBoneIndex];
	//DirectX::XMMATRIX result = bones[aBoneIndex].inverseBindPose * in.jointTransforms[aBoneIndex].GetMatrix();

	return DirectX::XMMatrixInverse(nullptr, currentJoint->inverseBindPose) * in.jointTransforms[aBoneIndex].GetMatrix();
	/*

	if (currentJoint->parentId == -1)
	{
		return result;
	}

	do
	{
		currentJoint = &bones[currentJoint->parentId];
		result = bones[aBoneIndex].inverseBindPose * result * in.jointTransforms[currentJoint->id].GetMatrix();
	} while (currentJoint->parentId != -1);

	result = bones[aBoneIndex].inverseBindPose * result * in.jointTransforms[currentJoint->id].GetMatrix();

	return Transform(result);
	*/
}
