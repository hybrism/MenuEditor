#include "pbr_functions.hlsli"
Texture2D aWorldPositionTexture : register(t1);
Texture2D aAlbedoTexture : register(t2);
Texture2D aNormalTexture : register(t3);
Texture2D aMaterialTexture : register(t5);


PixelOutput main(PixelInputType input)
{
    PixelOutput result;
   
    //float2 Resolution = float2(1584, 861);
    //float2 uv = input.position.xy / Resolution;
    
    
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
    
    float3 pointLightColor = 0; // <- The sum of all point lights.
 
 //   for (int i = 0; i < amountOfPointLights; i++)
 //   {
 //       pointLightColor += EvaluatePointLight(
	//		diffuseColor, specularColor, normal, Roughness,
	//		   myPointLights[i].pointLightsColor.rgb, myPointLights[i].pointLightsIntensity, myPointLights[i].pointLightsRange, myPointLights[i].pointLightsPosition.xyz,
	//		toEye.xyz, worldPosition.xyz);
 //   }
    
    
 //   float3 dir = directionalLightsDirection.xyz;
 //   float3 directionalLight = EvaluateDirectionalLight(
	//	diffuseColor, specularColor, normal, Roughness,
	//	directionalLightsColor.xyz, dir, toEye.xyz
	//);
        
    
    result.color.rgba = 1;
    result.color.rgb = pointLightColor.rgb;
    
    return result;
}