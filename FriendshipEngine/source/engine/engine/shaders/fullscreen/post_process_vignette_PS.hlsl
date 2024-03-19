#include "../common/common.hlsli"

cbuffer PostProcessBufferData : register(b5)
{
    PostProcessData postProcessData;
};

Texture2D postProcessTexture : register(t11);

float4 main(FullscreenVertexOutput input) : SV_Target
{
    float4 finalColor = float4(postProcessTexture.Sample(aDefaultSampler, input.uv.xy).rgb, 1);
    
    //float2 tex = float2(input.uv.x - 0.5f, input.uv.y - 0.5f);
    ////tex.x += (1.0f - sign(tex.x)) / 2.0f;
    ////tex.y += (1.0f - sign(tex.y)) / 2.0f;
    //tex.x = abs(tex.x);
    //tex.y = abs(tex.y);
    
    
    //// border
    //tex.x = pow(tex.x * postProcessData.vignette1, postProcessData.vignette2);
    //tex.y = pow(tex.y * postProcessData.vignette1, postProcessData.vignette2);
    
    //float dist = 1.0 - distance(float2(0.0, 0.0), tex);
    //dist = smoothstep(0.0, 0.5, dist); // softness
    
    //// combines vingette with center distance
    ////dist *= 1.0 - distance(float2(0.5, 0.5), tex);
    //finalColor.rgb *= dist;
    
    //Calculate edge curvature
    float2 curve = pow(abs(input.uv * 2.0f - 1.0f), float2(1.0f / postProcessData.vignetteCurvature, 1.0f / postProcessData.vignetteCurvature));
    //Compute distance to edge
    float edge = pow(length(curve), postProcessData.vignetteCurvature);
    //Compute vignette gradient and intensity
    float vignette = 1.0f - postProcessData.vignetteStrength * smoothstep(postProcessData.vignetteInner, postProcessData.vignetteOuter, edge);
    
    float3 color = postProcessData.vignetteColor.rgb * (1 - vignette);
    
    //Add vignette to the resulting texture
    finalColor.rgb += color;
    
    return finalColor;
}