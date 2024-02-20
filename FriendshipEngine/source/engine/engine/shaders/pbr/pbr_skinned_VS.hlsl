#include "../common/common.hlsli"

PixelInputType main(AnimatedVertexInputType input)
{
	PixelInputType output;
	
    float4x4 skinnedTransform = mul(bones[input.boneIndices.x], input.weights.x);
    skinnedTransform += mul(bones[input.boneIndices.y], input.weights.y);
    skinnedTransform += mul(bones[input.boneIndices.z], input.weights.z);
    skinnedTransform += mul(bones[input.boneIndices.w], input.weights.w);
	
    float4 pos = mul(skinnedTransform, float4(input.position.xyz, 1));
    float4 vertexObjectPos = mul(objectTransform, pos);
    float4 vertexViewPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexViewPos);
    
    
    const float3x3 objectToWorldRotation = float3x3(objectTransform._11, objectTransform._12, objectTransform._13,
                                                    objectTransform._21, objectTransform._22, objectTransform._23,
                                                    objectTransform._31, objectTransform._32, objectTransform._33);
    
    output.color = float4(input.color.xyz, 1.0f);
    output.binormal = mul(objectToWorldRotation, input.binormal);
    output.normal = mul(objectToWorldRotation, input.normal);
    output.tangent = mul(objectToWorldRotation, input.tangent);
    output.uv = input.uv;
    output.position = vertexClipPos;
    output.worldPosition = vertexObjectPos;
	
	return output;
}
