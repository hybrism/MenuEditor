#include "../common/common.hlsli"

Texture2D postProcessTexture : register(t11);

float4 main(FullscreenVertexOutput input) : SV_Target
{
    float4 finalColor = float4(postProcessTexture.Sample(aDefaultSampler, input.uv.xy).rgb, 1);
    
    // border
    float2 tex = float2(
        pow((input.uv.x - 0.5) * 1.67, 2.0), // shape
        pow((input.uv.y - 0.5) * 1.67, 2.0)
    );
    float dist = 1.0 - distance(float2(0.0, 0.0), tex);
    dist = smoothstep(0.0, 0.5, dist); // softness
    
    // combines vingette with center distance
    //dist *= 1.0 - distance(float2(0.5, 0.5), tex);
    finalColor.rgb *= dist;
    
    return finalColor;
}