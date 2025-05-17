#include "pbr_functions.hlsli"

/*
enum class PixelShaderTextureSlot : int
{
    Cubemap = 0,
    Albedo = 1,
	Normal = 2,
	VertexNormal = 3,
    Material = 4,
    Emissive = 5,
    Slot5 = 6, //TGE: Below 4 is the standard texture slots, keep above it! (: register( t4 ); in the shader)
	Slot6 = 7,  //"enum class GBufferTexture", in "GBuffer.h" uses enum slots 0 - 6
    ShadowMap = 8,
	DirectionalLight = 9,
	LightBound = 10,
};
*/

TextureCube aEnvironmentTexture : register(t0);
Texture2D aAlbedoTexture : register(t1);
Texture2D aNormalTexture : register(t2);
Texture2D aMaterialTexture : register(t4);
Texture2D aEffectsTexture : register(t5);

Texture2D aVertexPaintedAlbedoTextureR      : register(t16);
Texture2D aVertexPaintedNormalTextureR      : register(t17);
Texture2D aVertexPaintedMaterialTextureR    : register(t18);
Texture2D aVertexPaintedEffectsTextureR     : register(t19);


Texture2D aVertexPaintedAlbedoTextureG      : register(t20);
Texture2D aVertexPaintedNormalTextureG      : register(t21);
Texture2D aVertexPaintedMaterialTextureG    : register(t22);
Texture2D aVertexPaintedEffectsTextureG     : register(t23);


Texture2D aVertexPaintedAlbedoTextureB      : register(t24);
Texture2D aVertexPaintedNormalTextureB      : register(t25);
Texture2D aVertexPaintedMaterialTextureB    : register(t26);
Texture2D aVertexPaintedEffectsTextureB     : register(t27);


Texture2D aVertexPaintedAlbedoTextureA      : register(t28);
Texture2D aVertexPaintedNormalTextureA      : register(t29);
Texture2D aVertexPaintedMaterialTextureA    : register(t30);
Texture2D aVertexPaintedEffectsTextureA     : register(t31);

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

GBufferOutput main(DeferredPixelInputType input)
{
    float4 albedo = aAlbedoTexture.Sample(aDefaultSampler, input.uv).rgba;
    float3 normal = float3(aNormalTexture.Sample(aDefaultSampler, input.uv).xy, 1.0f);
    float3 material = aMaterialTexture.Sample(aDefaultSampler, input.uv).rgb;
    float2 effects = aEffectsTexture.Sample(aDefaultSampler, input.uv).rg;
    
    if (input.color.r > 0.0f)
    {
        albedo = lerp(albedo, aVertexPaintedAlbedoTextureR.Sample(aDefaultSampler, input.uv).rgba, input.color.r);
        normal = lerp(normal, aVertexPaintedNormalTextureR.Sample(aDefaultSampler, input.uv).rgb, input.color.r);
        material = lerp(material, aVertexPaintedMaterialTextureR.Sample(aDefaultSampler, input.uv).rgb, input.color.r);
        effects = lerp(effects, aVertexPaintedEffectsTextureR.Sample(aDefaultSampler, input.uv).rg, input.color.r);
    }
    if (input.color.g > 0.0f)
    {
        albedo = lerp(albedo, aVertexPaintedAlbedoTextureG.Sample(aDefaultSampler, input.uv).rgba, input.color.g);
        normal = lerp(normal, aVertexPaintedNormalTextureG.Sample(aDefaultSampler, input.uv).rgb, input.color.g);
        material = lerp(material, aVertexPaintedMaterialTextureG.Sample(aDefaultSampler, input.uv).rgb, input.color.g);
        effects = lerp(effects, aVertexPaintedEffectsTextureG.Sample(aDefaultSampler, input.uv).rg, input.color.g);
    }
    if (input.color.b > 0.0f)
    {
        albedo = lerp(albedo, aVertexPaintedAlbedoTextureB.Sample(aDefaultSampler, input.uv).rgba, input.color.b);
        normal = lerp(normal, aVertexPaintedNormalTextureB.Sample(aDefaultSampler, input.uv).rgb, input.color.b);
        material = lerp(material, aVertexPaintedMaterialTextureB.Sample(aDefaultSampler, input.uv).rgb, input.color.b);
        effects = lerp(effects, aVertexPaintedEffectsTextureB.Sample(aDefaultSampler, input.uv).rg, input.color.b);
    }
    if (input.color.a > 0.0f)
    {
        albedo = lerp(albedo, aVertexPaintedAlbedoTextureA.Sample(aDefaultSampler, input.uv).rgba, input.color.a);
        normal = lerp(normal, aVertexPaintedNormalTextureA.Sample(aDefaultSampler, input.uv).rgb, input.color.a);
        material = lerp(material, aVertexPaintedMaterialTextureA.Sample(aDefaultSampler, input.uv).rgb, input.color.a);
        effects = lerp(effects, aVertexPaintedEffectsTextureA.Sample(aDefaultSampler, input.uv).rg, input.color.a);
    }
    
    if (albedo.a < 0.1f)
    {
        discard;
    }
    
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
    output.normal = float4(0.5f + 0.5f * pixelNormal, 1.0f);
    output.vertexNormal = float4(input.normal, 1.0f);
    output.material = float4(material, 1.0f);
    output.effects = float4(effects, EntityIDToEffect(input.entityData.x)); // TODO: make use of the height value/ask graphics about what it is?
    
    return output;
}
