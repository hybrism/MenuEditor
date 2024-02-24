#include "pbr_functions.hlsli"

TextureCube aEnvironmentTexture : register(t0);
Texture2D aAlbedoTexture : register(t1);
Texture2D aNormalTexture : register(t2);
Texture2D aMaterialTexture : register(t3);
Texture2D aEffectsTexture : register(t4);

float3 expandNormal(float3 normalTexture)
{
    float3 normal = 2.0f * normalTexture - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
    return normalize(normal);
}

//ALBEDO:       R           G           B           A
//MATERIAL:     AO          Roughness   Metalness   N/A
//NORMAL:       X           Y           N/A         N/A
//EFFECTS:      Emissive    Height/DP?  Any         Any

PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    
    float4 albedo = aAlbedoTexture.Sample(aDefaultSampler, input.uv).rgba;
    
    if (albedo.a <= 0.1f)
    {
        discard;
    }
    
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
    float3 vertexNormal = input.normal;
    
    float3 cameraPosition = modelToWorld[3].xyz;
    float3 toEye = normalize(cameraPosition - input.worldPosition.xyz);
    
    float ambientOcclusion = material.r;
    float roughness = material.g;
    float metalness = material.b;
    float emissive = effects.r;
    
    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);
    
    float3 ambiance = EvaluateAmbiance(
		aEnvironmentTexture, pixelNormal, vertexNormal,
		toEye, roughness,
		ambientOcclusion, diffuseColor, specularColor
	);
    
    float3 dir = directionalLightsDirection.xyz;
    float3 directionalLight = EvaluateDirectionalLight(
		diffuseColor, specularColor, pixelNormal, roughness,
		directionalLightsColor.xyz, dir, toEye.xyz
	);
    
    float3 pointLightColor;
    for (int i = 0; i < amountOfPointLights; i++)
    {
        
        pointLightColor += EvaluatePointLight(
		    diffuseColor, specularColor, pixelNormal, roughness,
		    myPointLights[i].pointLightsColor.rgb, myPointLights[i].pointLightsIntensity, myPointLights[i].pointLightsRange, myPointLights[i].pointLightsPosition.xyz,
            toEye.xyz, input.worldPosition.xyz
        );
    }
    
    float3 emissiveAlbedo = albedo.rgb * emissive;
    float3 radiance = ambiance + directionalLight + emissiveAlbedo + pointLightColor;

    result.color.rgb = radiance;
    
    //result.color.rgb = pixelNormal;
    //result.color.rgb = vertexNormal;
    //result.color.rgb = material;
    //result.color.rgb = emissive;
    //result.color.rgb = input.worldPosition.xyz;

    result.color.a = albedo.a;
    
    return result;
}