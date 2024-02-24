#include "pbr_functions.hlsli"
Texture2D aWorldPositionTexture : register(t1);
Texture2D aAlbedoTexture : register(t2);
Texture2D aNormalTexture : register(t3);
Texture2D aMaterialTexture : register(t5);
Texture2D aShadowMapTexture : register(t8);


PixelOutput main(FullscreenVertexOutput input)
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
    
    float3 cameraPosition = modelToWorld[3].xyz;
    float3 toEye = normalize(cameraPosition - worldPosition.xyz);
    float3 normal = aNormalTexture.Sample(aDefaultSampler, uv).rgb;
 
    
    float3 dir = directionalLightsDirection.xyz;
    float3 directionalLight = EvaluateDirectionalLight(
		diffuseColor, specularColor, normal, Roughness,
		directionalLightsColor.xyz, dir, toEye.xyz
	);
    
    float4 wPos = float4(worldPosition.x, worldPosition.y, worldPosition.z, 1);
    float4 directionalLightProjectedPositionTemp = mul(directionalLightShadowSpaceToWorldSpace, wPos);
    float3 directionalLightProjectedPosition = directionalLightProjectedPositionTemp.xyz / directionalLightProjectedPositionTemp.w;
    float shadowFactor = 1.0f;
    
    if (clamp(directionalLightProjectedPosition.x, -1.0, 1.0) == directionalLightProjectedPosition.x &&
clamp(directionalLightProjectedPosition.y, -1.0, 1.0) == directionalLightProjectedPosition.y)
    {
        float computedZ = directionalLightProjectedPosition.z;
        float shadowMapZ = aShadowMapTexture.Sample(aDefaultSampler, 0.5f + float2(0.5f, -0.5f) * directionalLightProjectedPosition.xy);
        float bias = 0.001;
        shadowFactor = (computedZ < shadowMapZ + bias);
    }
    
    result.color = float4(directionalLight.rgb * shadowFactor, 1);
    
    return result;
}