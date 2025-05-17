#include "../vfx/vfx_common.hlsli"

struct DebugVertexInputType
{
    float3 position : POSITION;
    float4 color : COLOR;
};

ParticlePixelInputType main(DebugVertexInputType input)
{
    ParticlePixelInputType output;

    float2 targetViewport = float2(resolution.z, resolution.w);
    float2 pos = (input.position.xy / targetViewport.xy) * 2 - float2(1, 1);
    
    output.color = float4(input.color.rgb, 1.f);
    output.uv = float2(0, 0);
    output.position = float4(pos.x, -pos.y, 0, 1);
    output.worldPosition = float4(pos.x, -pos.y, 0, 1);
    
    return output;
}
