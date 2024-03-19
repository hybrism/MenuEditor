#include "../common/common.hlsli"
Texture2D postProcessTexture : register(t11);

float3 s_curve(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

float3 tonemap_s_gamut3_cine(float3 c)
{
    // based on Sony's s gamut3 cine
    float3x3 fromSrgb = float3x3(
        +0.6456794776, +0.2591145470, +0.0952059754,
        +0.0875299915, +0.7596995626, +0.1527704459,
        +0.0369574199, +0.1292809048, +0.8337616753);

    float3x3 toSrgb = float3x3(
        +1.6269474099, -0.5401385388, -0.0868088707,
        -0.1785155272, +1.4179409274, -0.2394254004,
        +0.0444361150, -0.1959199662, +1.2403560812);

    return mul(toSrgb, s_curve(mul(fromSrgb, c)));
}

float3 linearToneMapping(float3 color, float gamma)
{
    float exposure = 1.0f;
    color = clamp(exposure * color, 0.0f, 1.0f);
    color = pow(color, float3(1.0f / gamma, 1.0f / gamma, 1.0f / gamma));
    return color;
}

float3 simpleReinhardToneMapping(float3 color, float gamma)
{
    float exposure = 1.5f;
    color *= exposure / (1.0f + color / exposure);
    color = pow(color, float3(1.0f / gamma, 1.0f / gamma, 1.0f / gamma));
    return color;
}

float3 lumaBasedReinhardToneMapping(float3 color, float gamma)
{
    float luma = dot(color, float3(0.2126f, 0.7152f, 0.0722f));
    float toneMappedLuma = luma / (1.0f + luma);
    color *= toneMappedLuma / luma;
    color = pow(color, float3(1.0f / gamma, 1.0f / gamma, 1.0f / gamma));
    return color;
}

float3 whitePreservingLumaBasedReinhardToneMapping(float3 color, float gamma)
{
    float white = 2.;
    float luma = dot(color, float3(0.2126f, 0.7152f, 0.0722f));
    float toneMappedLuma = luma * (1.0f + luma / (white * white)) / (1.0f + luma);
    color *= toneMappedLuma / luma;
    color = pow(color, float3(1.0f / gamma, 1.0f / gamma, 1.0f / gamma));
    return color;
}

float3 RomBinDaHouseToneMapping(float3 color, float gamma)
{
    color = exp(-1.0f / (2.72f * color + 0.15f));
    color = pow(color, float3(1.0f / gamma, 1.0f / gamma, 1.0f / gamma));
    return color;
}

float3 filmicToneMapping(float3 color)
{
    color = max(float3(0.0f, 0.0f, 0.0f), color - float3(0.004f, 0.004f, 0.004f));
    color = (color * (6.2f * color + 0.5f)) / (color * (6.2f * color + 1.7f) + 0.06f);
    return color;
}

// https://lettier.github.io/3d-game-shaders-for-beginners/gamma-correction.html
float4 main(FullscreenVertexOutput input) : SV_Target
{
    float4 output = float4(postProcessTexture.Sample(aDefaultSampler, input.uv.xy).rgb, 1.0f);
    //output.rgb = s_curve(output.rgb);
    
    //float gamma = 2.2f; // 2.2f for sRGB, 1.0f for linear
    
    //// sRGB to linear
    //output.rgb = pow(output.rgb, gamma);
    
    ////output.rgb = tonemap_s_gamut3_cine(output.rgb);
    ////output.rgb = tonemap_s_gamut3_cine(output.rgb);
    
    
    //float saturation = 1.1f;
    //float contrast = 1.1f;
    //float exposure = 0.0f;
    
    ////SATURATION
    //float luminance = dot(float3(0.2126, 0.7152, 0.0722), output.rgb);
    //output.rgb = luminance + saturation * (output.rgb - luminance);
    
    ////EXPOSURE
    //output.rgb = exp2(exposure) * output.rgb;
     
    ////CONTRAST
    //output.rgb = 0.18f * pow(output.rgb / 0.18f, float3(contrast, contrast, contrast));
    
    //// linear to sRGB 
    //output.rgb = pow(output.rgb, 1.0f / gamma);
    
    ////finalColor.rgb = tonemap_s_gamut3_cine(finalColor.rgb);
    
    //float3x3 toSrgb = float3x3(
    //    +1.6269474099, -0.5401385388, -0.0868088707,
    //    -0.1785155272, +1.4179409274, -0.2394254004,
    //    +0.0444361150, -0.1959199662, +1.2403560812);
    
    //float3x3 fromSrgb = float3x3(
    //    +0.6456794776, +0.2591145470, +0.0952059754,
    //    +0.0875299915, +0.7596995626, +0.1527704459,
    //    +0.0369574199, +0.1292809048, +0.8337616753);
    
    
    //output.rgb = simpleReinhardToneMapping(output.rgb, gamma);
    //output.rgb = lumaBasedReinhardToneMapping(output.rgb, gamma);
    //output.rgb = whitePreservingLumaBasedReinhardToneMapping(output.rgb, gamma);
    //output.rgb = RomBinDaHouseToneMapping(output.rgb, gamma);
    //output.rgb = tonemap_s_gamut3_cine(output.rgb);
    //output.rgb = mul(toSrgb, output.rgb);
    //output.rgb = linearToneMapping(output.rgb, gamma);
    
    //output.rgb = linear_to_srgb(output.rgb);
    //output.rgb = tonemap_s_gamut3_cine(output.rgb);
    
    output.a = 1.0f;
    
    return output;
}
