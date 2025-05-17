#include "vfx_common.hlsli"

Texture2D aDepthTexture : register(t14);

PixelOutput main(VfxPixelInputType input)
{
    PixelOutput result;
    
    float4 emissiveColor = float4(0.479f, 0.99f, 1.f, 1.f);
    
    float power = (cos(time * 4) + 1) * exp(3);
    float opacity = pow(input.uv.y, power);
    
    result.color.rgb = emissiveColor.rgb;
    
    float2 screenUV = input.position.xy / resolution.zw;
    float1 screenDepth = aDepthTexture.Sample(aDefaultSampler, screenUV).r;
    
    float effectAlpha = exp(-depthFadeK * max(0.0, screenDepth - input.depth));
    
    result.color.a = opacity;
    result.color.a *= clamp(1 - effectAlpha, 0, 1);
    
    return result;
}
