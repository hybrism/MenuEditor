#include "../common/common.hlsli"

cbuffer PostProcessBufferData : register(b5)
{
    PostProcessData postProcessData;
};

Texture2D postProcessTexture : register(t11);

float4 main(FullscreenVertexOutput input) : SV_Target
{
    float4 result = 0;
    
    float2 scaledUv = input.uv * ((postProcessData.downScaleLevel / 100) + 1);
    float2 pixelOffset = float2(ddx(scaledUv.x), ddy(scaledUv.y));
    
    float3 p00 = postProcessTexture.Sample(aClampingSampler, input.uv + pixelOffset * float2(-0.5f, -0.5f)).rgb;
    float3 p01 = postProcessTexture.Sample(aClampingSampler, input.uv + pixelOffset * float2(-0.5f, 0.5f)).rgb;
    float3 p10 = postProcessTexture.Sample(aClampingSampler, input.uv + pixelOffset * float2(0.5f, -0.5f)).rgb;
    float3 p11 = postProcessTexture.Sample(aClampingSampler, input.uv + pixelOffset * float2(0.5f, 0.5f)).rgb;
   
    
    float3 color = 0.25f * (p00 + p01 + p10 + p11);
    
    //SATURATION
    float luminance = dot(float3(0.2126, 0.7152, 0.0722), color);
    color = luminance + postProcessData.saturation * (color - luminance);
    
    //EXPOSURE
    color = exp2(postProcessData.exposure) * color;
     
    //CONTRAST
    color = 0.18f * pow(color / 0.18f, postProcessData.contrast);
    
    //TINT
    color = color * postProcessData.tint;
    
    //BLACKPOINT
    color = max(0.0f, color - postProcessData.blackPoint);
    
    result.rgb = color.rgb;
    result.a = postProcessData.alphaBlendLevel;
    
    return result;
}