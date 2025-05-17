#include "../common/common.hlsli"

cbuffer PostProcessBufferData : register(b5)
{
    PostProcessData postProcessData;
};

Texture2D postProcessTexture : register(t11);

float4 main(FullscreenVertexOutput input) : SV_Target
{
    float4 finalColor = float4(postProcessTexture.Sample(aDefaultSampler, input.uv.xy).rgb, 1);
    
    float2 curve = pow(abs(input.uv * 2.0f - 1.0f), float2(1.0f / postProcessData.vignetteCurvature, 1.0f / postProcessData.vignetteCurvature));
    float edge = pow(length(curve), postProcessData.vignetteCurvature);
    float vignette = 1.0f - postProcessData.vignetteStrength * smoothstep(postProcessData.vignetteInner, postProcessData.vignetteOuter, edge);
    
    finalColor.rgb *= vignette;
    finalColor.rgb += postProcessData.vignetteColor * (1 - vignette);
    
    return finalColor;
}