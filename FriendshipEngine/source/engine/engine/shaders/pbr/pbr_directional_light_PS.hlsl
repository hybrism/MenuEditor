#include "pbr_functions.hlsli"
TextureCube aEnvironmentTexture : register(t0);
Texture2D aWorldPositionTexture : register(t1);
Texture2D aAlbedoTexture : register(t2);
Texture2D aNormalTexture : register(t3);
Texture2D aVertexNormalTexture : register(t4);
Texture2D aMaterialTexture : register(t5);
Texture2D aEffectsTexture : register(t6);

Texture2D aShadowMapTexture : register(t8);




PixelOutput main(FullscreenVertexOutput input)
{
   
    PixelOutput result;
    
    float2 Resolution = resolution.zw;
   
    float2 uv = input.position.xy / Resolution.xy;
    
    //float3 material = aMaterialTexture.Sample(aDefaultSampler, uv.xy).rgb;
    
    //float Metalness = material.b;
    //float Roughness = material.g;
    
    
    float3 worldPosition = aWorldPositionTexture.Sample(aDefaultSampler, uv).rgb;

    //float3 albedo = aAlbedoTexture.Sample(aDefaultSampler, uv).rgb;

    //float3 specularColor = lerp((float3) 0.04f, albedo, Metalness);
    //float3 diffuseColor = lerp((float3) 0.00f, albedo, 1 - Metalness);
    
    //float3 cameraPosition = modelToWorld[3].xyz;
    //float3 toEye = normalize(cameraPosition - worldPosition.xyz);
    //float3 normal = aNormalTexture.Sample(aDefaultSampler, uv).rgb;
 

    
    float4 wPos = float4(worldPosition.x, worldPosition.y, worldPosition.z, 1);
    float4 directionalLightProjectedPositionTemp = mul(directionalLightShadowSpaceToWorldSpace, wPos);
    float3 directionalLightProjectedPosition = directionalLightProjectedPositionTemp.xyz / directionalLightProjectedPositionTemp.w;
    float shadowFactor = 1.0f;
    float bias = 0.0001;
	
    int xDiff = 20;
    int zDiff = 20;
    
    if (clamp(directionalLightProjectedPosition.x, -1.0, 1.0) == directionalLightProjectedPosition.x &&
clamp(directionalLightProjectedPosition.y, -1.0, 1.0) == directionalLightProjectedPosition.y)
    {
        float computedZ = directionalLightProjectedPosition.z;
        float2 texCoords = (0.5f + float2(0.5f, -0.5f) * directionalLightProjectedPosition.xy);
		//texCoords *= float2(3840.f / Resolution.x, 2160.f / Resolution.y).xy;
		
        float mapWidth, mapHeight, mapNumOfLevels = 0;
        aShadowMapTexture.GetDimensions(0, mapWidth, mapHeight , mapNumOfLevels);
		
        float2 mapOffset = float2(1.f / mapWidth, 1.f / mapHeight).xy;
        const float offsetScale = 0.0005f;
		
		int sampleAmount = 16;
 
		
        for (int x = -sampleAmount / 2; x < sampleAmount / 2; x++)
        {
            for (int y = -sampleAmount / 2; y < sampleAmount / 2; y++)
            {
                float2 uvOffset = float2(x * mapOffset.x, y * mapOffset.y);
				
                float shadowMapZ = aShadowMapTexture.Sample(aDefaultSampler, texCoords + uvOffset * offsetScale);
                shadowFactor += (computedZ < shadowMapZ + bias);
            }
        }
        shadowFactor /= sampleAmount*sampleAmount;
    }
    result.color = clamp(shadowFactor, useShadows , 1);
    
    return result;
}

