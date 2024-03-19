#include "../common/common.hlsli"

PixelInputType main(AnimatedVertexInstancedInputType input)
{
	PixelInputType output;
	
    float4x4 skinnedTransform = mul(bones[input.boneIndices.x], input.weights.x);
    skinnedTransform += mul(bones[input.boneIndices.y], input.weights.y);
    skinnedTransform += mul(bones[input.boneIndices.z], input.weights.z);
    skinnedTransform += mul(bones[input.boneIndices.w], input.weights.w);
	
    float4 pos = mul(skinnedTransform, float4(input.position.xyz, 1));
    float4 vertexObjectPos = mul(input.instanceTransform, pos);
    float4 vertexViewPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexViewPos);
	
    output.color = float4(input.color.xyz, 0);
    output.binormal = input.binormal;
    output.normal = input.normal;
    output.tangent = input.tangent;
    output.uv = input.uv;
    output.position = vertexClipPos;
    output.worldPosition = vertexObjectPos;
    output.color = float4(0, 0, 0, 0);
	
	return output;
}
