#include "../vfx/vfx_common.hlsli"

struct DebugVertexInputType
{
    float3 position : POSITION;
    float4 color : COLOR;
};

ParticlePixelInputType main(DebugVertexInputType input)
{
    ParticlePixelInputType output;

    float4 vertexObjectPos = float4(input.position.xyz, 1);
    float4 vertexViewPos = mul(modelToWorld, float4(vertexObjectPos.xyz, 1.0f));
    float4 vertexClipPos = mul(worldToClipMatrix, float4(vertexViewPos.xyz, 1.0f));
	
    output.color = float4(input.color.xyz, 1.f);
    output.uv = float2(0, 0);
    output.position = vertexClipPos;
    output.worldPosition = vertexObjectPos;
    
    return output;
}
