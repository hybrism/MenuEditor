#include "pbr_functions.hlsli"

TextureCube aEnvironmentTexture : register(t0);
Texture2D aAlbedoTexture : register(t1);
Texture2D aNormalTexture : register(t2);
Texture2D aMaterialTexture : register(t3);
Texture2D aEffectsTexture : register(t4);

// INPUT
//ALBEDO:       R           G           B                     A
//MATERIAL:     AO          Roughness   Metalness             N/A
//NORMAL:       X           Y           N/A                   N/A
//EFFECTS:      Emissive    Height/DP?  AlbedoOverride        Any

float3 expandNormal(float3 normalTexture)
{
    float3 normal = 2.0f * normalTexture - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
    return normalize(normal);
}

GBufferOutput main(PixelInputType input)
{
    float4 albedo = aAlbedoTexture.Sample(aDefaultSampler, input.uv).rgba;
    
    if (albedo.a < 0.1f) { discard; }
    
    float3 normal = aNormalTexture.Sample(aDefaultSampler, input.uv).xyz;
    float3 material = aMaterialTexture.Sample(aDefaultSampler, input.uv).rgb;
    float2 effects = aEffectsTexture.Sample(aDefaultSampler, input.uv).rg;
    
    normal = expandNormal(normal);
    
    float3x3 TBN = float3x3(
        normalize(input.tangent.xyz),
        normalize(-input.binormal.xyz),
        normalize(input.normal.xyz)
    );
    
    TBN = transpose(TBN);
    float3 pixelNormal = normalize(mul(TBN, normal));
    
    GBufferOutput output;
    output.worldPosition = input.worldPosition;
    output.albedo = albedo;
    output.normal = float4(pixelNormal, 1.0f);
    output.vertexNormal = float4(input.normal, 1.0f);
    output.material = float4(material, 1.0f);
    output.effects = float4(effects, 0.f, 0.f); // a is unused, TODO: make use of the height value/ask graphics about what it is?
    
    return output;
}
