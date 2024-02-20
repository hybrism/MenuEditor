#include "common.hlsli"

GBufferOutput main(PixelInputType input)
{
    GBufferOutput output;
    output.worldPosition = input.worldPosition;
    output.albedo = float4(0.4f, 0.8f, 1, 1);
    output.normal = float4(input.normal.xyz, 1);
    output.vertexNormal = float4(input.normal, 1);
    output.material = float4(1, 1, 1, 1);
    output.effects = float4(0, 0, 1, 0); // ba are unused, TODO: make use of the height value/ask graphics about what it is?
    
    return output;
}
