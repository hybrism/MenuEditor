#include "vfx_common.hlsli"

VfxPixelInputType main(VfxVertexInputType input)
{
    VfxPixelInputType output;

    float4 vertexObjectPos = mul(input.instanceTransform, float4(input.position.xyz, 1));
    float4 vertexViewPos = mul(modelToWorld, float4(vertexObjectPos.xyz, 1.0f));
    float4 vertexClipPos = mul(worldToClipMatrix, float4(vertexViewPos.xyz, 1.0f));
	
    
    const float3x3 objectToWorldRotation = float3x3(input.instanceTransform._11, input.instanceTransform._12, input.instanceTransform._13,
                                                    input.instanceTransform._21, input.instanceTransform._22, input.instanceTransform._23,
                                                    input.instanceTransform._31, input.instanceTransform._32, input.instanceTransform._33);
    
    output.color = float4(input.color.xyz, 1.0f);
    output.binormal = mul(objectToWorldRotation, input.binormal);
    output.normal = mul(objectToWorldRotation, input.normal);
    output.tangent = mul(objectToWorldRotation, input.tangent);
    output.uv = input.uv;
    output.position = vertexClipPos;
    output.worldPosition = vertexObjectPos;
    output.time = 0.0f;
    output.color = float4(0, 0, 0, 1);
    
    output.depth = vertexClipPos.z / vertexClipPos.w;
    
    return output;
}
