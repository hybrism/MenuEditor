#include "vfx_common.hlsli"

Texture2D aDepthTexture : register(t14);

PixelOutput main(VfxPixelInputType input)
{
    PixelOutput result;
    float4 vertexObjectNormal = float4(input.normal, 0.0);
    float4 vertexWorldNormal = mul(modelToWorld, vertexObjectNormal);
    
    float1 depth = aDepthTexture.Sample(aDefaultSampler, float2(0.5f, 0.5f)).r;
    
    float4 emissiveColor = float4(0.551f, 0.97f, 1.f, 1.f);
    float sinusCurve = clamp(cos(input.time * 5) + 1, 0, 1);
    float rim = pow(input.uv.g, 15.f);
    float middleOpacity = clamp(pow(input.uv.g, exp(1)), 0, 0.664f) * 0.07f;
    
    float total = rim + middleOpacity;
    total *= sinusCurve;
    
    result.color.rgb = emissiveColor.rgb;
    result.color.a = total;
    
    //result.color.rgb = float3(depth, depth, depth);
    //result.color.a = 1.0f;
    
    return result;
}
