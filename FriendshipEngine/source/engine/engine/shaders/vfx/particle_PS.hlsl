#include "vfx_common.hlsli"

Texture2D aDepthTexture : register(t14);

PixelOutput main(ParticlePixelInputType input)
{
    PixelOutput result;
    
    float2 screenUV = input.position.xy / resolution.zw;
    float1 screenDepth = aDepthTexture.Sample(aDefaultSampler, screenUV).r;  
    
    float effectAlpha = exp(-depthFadeK * max(0.0, screenDepth - input.depth));
    
    float dx = 1 - fmod(abs(input.uv.x - 0.5f), 0.5f) * 2.0f;
    float dy = 1 - fmod(abs(input.uv.y - 0.5f), 0.5f) * 2.0f;
    float distFromCenter = step(1.0f, saturate(dx * dx + dy * dy));
    
    result.color.rgb = input.color.rgb;
    result.color.a = distFromCenter * clamp(1 - effectAlpha, 0, 1);
    result.color.a *= smoothstep(0, 0.15, input.time);
    result.color.a *= smoothstep(1, 0.85, input.time);
    
    return result;
}
