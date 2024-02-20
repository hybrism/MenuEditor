#include "vfx_common.hlsli"

PixelOutput main(VfxPixelInputType input)
{
    PixelOutput result;
    float4 vertexObjectNormal = float4(input.normal, 0.0);
    float4 vertexWorldNormal = mul(modelToWorld, vertexObjectNormal);
    
    float4 emissiveColor = float4(0.551f, 0.97f, 1, 1);
    float sinusCurve = clamp(cos(input.time * 5) + 1, 0, 1);
    float rim = pow(input.uv.g, 15);
    float middleOpacity = clamp(pow(input.uv.g, exp(1)), 0, 0.664f) * 0.07f;
    
    float total = rim + middleOpacity;
    total *= sinusCurve;
    
    result.color.rgb = emissiveColor.rgb;
    result.color.a = total;
    
    return result;
}
