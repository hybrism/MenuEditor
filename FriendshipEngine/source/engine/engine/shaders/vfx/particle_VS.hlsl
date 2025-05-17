#include "vfx_common.hlsli"

ParticlePixelInputType main(ParticleVertexInputType input)
{
    ParticlePixelInputType output;
    
    const float3x3 cameraRotation = inverse(float3x3(modelToWorld._11, modelToWorld._12, modelToWorld._13,
                                              modelToWorld._21, modelToWorld._22, modelToWorld._23,
                                              modelToWorld._31, modelToWorld._32, modelToWorld._33));
    const float4x4 finalInstanceTransform = float4x4(cameraRotation._11, cameraRotation._12, cameraRotation._13, input.instanceTransform._14,
                                                      cameraRotation._21, cameraRotation._22, cameraRotation._23, input.instanceTransform._24,
                                                      cameraRotation._31, cameraRotation._32, cameraRotation._33, input.instanceTransform._34,
                                                      0, 0, 0, input.instanceTransform._43);
    
    const float4x4 scaleTransform = float4x4(input.instanceTransform._11, 0, 0, 0,
                                             0, input.instanceTransform._22, 0, 0,
                                             0, 0, input.instanceTransform._33, 0,
                                             0, 0, 0, 1);
    float4 vertexObjectPos = mul(scaleTransform, float4(input.position.xyz, 1));
    vertexObjectPos = mul(finalInstanceTransform, vertexObjectPos);
    float4 vertexViewPos = mul(modelToWorld, float4(vertexObjectPos.xyz, 1.0f));
    float4 vertexClipPos = mul(worldToClipMatrix, float4(vertexViewPos.xyz, 1.0f));
    
    output.color = input.color;
    output.uv = input.uv;
    output.position = vertexClipPos;
    output.worldPosition = vertexObjectPos;
    output.time = input.time;
    output.depth = vertexClipPos.z / vertexClipPos.w;
    
    return output;
}
