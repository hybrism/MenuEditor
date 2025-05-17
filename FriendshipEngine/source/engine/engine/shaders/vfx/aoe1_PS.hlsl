#include "vfx_common.hlsli"

Texture2D aDepthTexture : register(t14);

PixelOutput main(VfxPixelInputType input)
{
    PixelOutput result;
    
    float4 emissiveColor = float4(0.551f, 0.97f, 1.f, 1.f);
    float sinusCurve = clamp(cos(time * 5) + 1, 0, 1);
    float rim = pow(input.uv.g, 15.f);
    float middleOpacity = clamp(pow(input.uv.g, exp(1)), 0, 0.664f) * 0.07f;
    
    float total = rim + middleOpacity;
    total *= sinusCurve;
    
    result.color.rgb = emissiveColor.rgb;
    result.color.a = total;
    
    return result;
}
