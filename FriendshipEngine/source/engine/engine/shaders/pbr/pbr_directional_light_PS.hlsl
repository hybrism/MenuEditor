#include "pbr_functions.hlsli"
Texture2D aWorldPositionTexture : register(t1);
Texture2D aAlbedoTexture : register(t2);
Texture2D aNormalTexture : register(t3);
Texture2D aMaterialTexture : register(t5);


PixelOutput main(DeferredVertexOutput input)
{
    PixelOutput result;
    
    float2 Resolution = resolution.zw;
   
    float2 uv = input.position.xy / Resolution.xy;
    
    float3 material = aMaterialTexture.Sample(aDefaultSampler, uv.xy).rgb;
    
    float Metalness = material.r;
    float Roughness = material.g;
    
    
    float3 worldPosition = aWorldPositionTexture.Sample(aDefaultSampler, uv).rgb;

    float3 albedo = aAlbedoTexture.Sample(aDefaultSampler, uv).rgb;

    float3 specularColor = lerp((float3) 0.04f, albedo, Metalness);
    float3 diffuseColor = lerp((float3) 0.00f, albedo, 1 - Metalness);
    float3 toEye = normalize(CameraPosition.xyz - worldPosition.xyz);
    float3 normal = aNormalTexture.Sample(aDefaultSampler, uv).rgb;
 
    
    float3 dir = directionalLightsDirection.xyz;
    result.color.rgba = 1;
    result.color.rgb = dir.rgb;
    float3 directionalLight = EvaluateDirectionalLight(
		diffuseColor, specularColor, normal, Roughness,
		directionalLightsColor.xyz, dir, toEye.xyz
	);
        
    
    result.color.rgba = 1;
    result.color.rgb = directionalLight.rgb;
    
    return result;
}