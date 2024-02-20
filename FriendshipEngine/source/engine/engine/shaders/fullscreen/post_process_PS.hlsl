#include "../common/Common.hlsli"

Texture2D postProcessTexture : register(t11);

float4 main(PixelInputType input) : SV_Target
{
    float4 result;
   
    
    float2 scaledUv = input.uv * ((DownScaleLevel / 100) + 1);
    
    float2 pixelOffset = float2(ddx(scaledUv.x), ddy(scaledUv.y));
    
    
    float3 p00 = postProcessTexture.Sample(aClampingSampler, input.uv + pixelOffset * float2(-0.5f, -0.5f)).rgb;
    float3 p01 = postProcessTexture.Sample(aClampingSampler, input.uv + pixelOffset * float2(-0.5f, 0.5f)).rgb;
    float3 p10 = postProcessTexture.Sample(aClampingSampler, input.uv + pixelOffset * float2(0.5f, -0.5f)).rgb;
    float3 p11 = postProcessTexture.Sample(aClampingSampler, input.uv + pixelOffset * float2(0.5f, 0.5f)).rgb;
   
    
    float3 color;
    
    color.rgb = 0.25f * (p00 + p01 + p10 + p11);
    result.a = AlphaBlendLevel;
    
    //SATURATION
    float saturation = buffSaturation;
    float luminance = dot(float3(0.2126, 0.7152, 0.0722), color);
    color = luminance + saturation * (color - luminance);
    
    //EXPOSURE
    float exposure = buffExposure;
    color = exp2(exposure) * color;
     
    //CONTRAST
    float3 contrast = buffContrast;
    color = 0.18f * pow(color / 0.18f, contrast);
    
    //TINT
    float3 tint = buffTint;
    color = color * tint;
    
    //BLACKPOINT
    float3 blackPoint = buffBlackPoint;
    color = max(0.0f, color - blackPoint);
    
    result.rgb = color.rgb;
    
    return result;
}