#include "pbr_functions.hlsli"

TextureCube aEnvironmentTexture : register(t0);
Texture2D aWorldPositionTexture : register(t1);
Texture2D aAlbedoTexture : register(t2);
Texture2D aNormalTexture : register(t3);
Texture2D aVertexNormalTexture : register(t4);
Texture2D aMaterialTexture : register(t5);
Texture2D aEffectsTexture : register(t6);

Texture2D aShadowMapTexture : register(t8);
Texture2D aDirectionlaLightTexture : register(t9);
Texture2D aPointLightTexture : register(t10);

float3 expandNormal(float3 normalTexture)
{
    float3 normal = 2.0f * normalTexture - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
    return normalize(normal);
}

//ALBEDO:       R           G           B                     A
//MATERIAL:     AO          Roughness   Metalness             N/A
//NORMAL:       X           Y           N/A                   N/A
//EFFECTS:      Emissive    Height/DP?  AlbedoOverride        Any

PixelOutput main(FullscreenVertexOutput input)
{
    PixelOutput result;
    
 //   //ALBEDO
 //   float4 albedo = aAlbedoTexture.Sample(aDefaultSampler, input.uv).rgba;
    
 //   if (albedo.a <= 0.0f)
 //   {
 //       discard;
 //       //result.color = float4(0.f, 0.f, 0.f, 0.f);
 //       //return result;
 //   }
        
 //   // Texture samples
 //   float3 worldPosition = aWorldPositionTexture.Sample(aDefaultSampler, input.uv).xyz;
 //   float3 pixelNormal = aNormalTexture.Sample(aDefaultSampler, input.uv).xyz;
 //   float3 vertexNormal = aVertexNormalTexture.Sample(aDefaultSampler, input.uv).xyz;
 //   float3 material = aMaterialTexture.Sample(aDefaultSampler, input.uv).rgb;
 //   float3 effects = aEffectsTexture.Sample(aDefaultSampler, input.uv).rgb;
    
 //   // Texture extraction
 //   float1 ambientOcclusion = material.r;
 //   float1 roughness = material.g;
 //   float1 metalness = material.b;
 //   float1 emissive = effects.r;
 //   float3 toEye = normalize(CameraPosition.xyz - worldPosition.xyz);
    
 //   float3 specularColor = lerp((float3)0.04f, albedo.rgb, metalness);
 //   float3 diffuseColor = lerp((float3)0.00f, albedo.rgb, 1 - metalness);
    
 //   float3 ambiance = EvaluateAmbiance(
	//	aEnvironmentTexture, pixelNormal, vertexNormal,
	//	toEye, roughness,
	//	ambientOcclusion, diffuseColor, specularColor
	//);
    
 //   float3 dir = directionalLightDir.xyz;
 //   float3 directionalLight = EvaluateDirectionalLight(
	//	diffuseColor, specularColor, pixelNormal, roughness,
	//	directionalLightColor.xyz, dir, toEye.xyz
	//);
    
 //   float3 pointLightColor;
 //   for (int i = 0; i < numberOfPointLights; i++)
 //   {
 //       pointLightColor += EvaluatePointLight(
	//	diffuseColor, specularColor, pixelNormal, roughness,
	//	pointLights[i].color.xyz, pointLights[i].color.w, pointLights[i].range, pointLights[i].position.xyz
 //       , toEye.xyz, worldPosition);
 //   }
    
 //   float3 emissiveAlbedo = albedo.rgb * emissive;
 //   float3 radiance = ambiance + directionalLight + emissiveAlbedo + pointLightColor;

 //   result.color.rgb = radiance;
 //   result.color.a = albedo.a;
        
    
    
    
    float2 scaledUV = input.uv;
    
    //ALBEDO
    float4 albedo = aAlbedoTexture.Sample(aDefaultSampler, scaledUV).rgba;
    
    if (albedo.a <= 0.0f)
    {
        discard;
        result.color = float4(0.f, 0.f, 0.f, 0.f);
        return result;
    }
    
    
    float3 worldPosition = aWorldPositionTexture.Sample(aDefaultSampler, scaledUV).xyz;
    float3 pixelNormal = UnpackNormal(aNormalTexture.Sample(aDefaultSampler, scaledUV).xyz);
    float3 vertexNormal = aVertexNormalTexture.Sample(aDefaultSampler, scaledUV).xyz;
    
    float3 cameraPosition = GetCameraPosition();
    float3 toEye = normalize(cameraPosition - worldPosition.xyz);
    
    //MATERIAL
    float3 material = aMaterialTexture.Sample(aDefaultSampler, scaledUV).rgb;
    float ambientOcclusion = material.r;
    float roughness = material.g;
    float metalness = material.b;
    
    //EMISSIVE
    float4 effects = aEffectsTexture.Sample(aDefaultSampler, scaledUV).rgba;
    float1 emissive = effects.r;
    float3 emissiveAlbedo = albedo.rgb * emissive;
    //float1 heightDP = effects.g;
    
    //LIGHT
    float4 pointLight = aPointLightTexture.Sample(aDefaultSampler, scaledUV).rgba;
    float4 shadow = aDirectionlaLightTexture.Sample(aDefaultSampler, scaledUV).rgba;
    
    
    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);
    
    float3 ambiance = EvaluateAmbiance(
		aEnvironmentTexture, pixelNormal, vertexNormal,
		toEye, roughness, ambientOcclusion,
        diffuseColor, specularColor
	) * ambientLightIntensity;
    
        
    float3 dir = directionalLightsDirection.xyz * -1.f;
    float3 directionalLight = EvaluateDirectionalLight(
		diffuseColor, specularColor, pixelNormal, roughness,
		directionalLightsColor.xyz, dir, toEye.xyz
	) * directionalLightsIntensity;
    

    //float4 wPos = float4(worldPosition.x, worldPosition.y, worldPosition.z, 1);
    //float4 directionalLightProjectedPositionTemp = mul(directionalLightShadowSpaceToWorldSpace, wPos);
    //float3 directionalLightProjectedPosition = directionalLightProjectedPositionTemp.xyz / directionalLightProjectedPositionTemp.w;
    //float shadowFactor = 1.0f;
    
    float3 radiance = ambiance + pointLight.rgb + directionalLight.rgb * shadow.r * (1.0f - emissive) + emissiveAlbedo;
    result.color.rgb = radiance;
    result.color.a = albedo.a;
    
    switch (renderMode)
    {
        case 0:
            break;
        case 1:
            result.color = float4(worldPosition, 1.f);
            break;
        case 2:
            result.color = albedo;
            break;
        case 3:
            result.color = float4(aNormalTexture.Sample(aDefaultSampler, scaledUV).xyz, 1.f);
            break;
        case 4:
            result.color = float4(vertexNormal, 1.f);
            break;
        case 5:
            result.color = float4(material, 1.f);
            break;
        case 6:
            result.color = effects;
            //result.color.a = 1.0f; // temp?
            break;
        default:
            break;
    }
    
	return result;
}