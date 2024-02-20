#include "../common/common.hlsli"
Texture2D sampledTexture : register(t0);

float4 main(DeferredVertexOutput input) : SV_Target
{
    float3 resource = sampledTexture.Sample(aDefaultSampler, input.uv.xy).rgb;
    
    return float4(resource.rgb, 1.0f);
}