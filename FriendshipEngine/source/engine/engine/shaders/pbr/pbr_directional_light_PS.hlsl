#include "pbr_functions.hlsli"
Texture2D aWorldPositionTexture : register(t1);

Texture2D aShadowMapTexture : register(t8);
Texture2D aDepthTexture : register(t14);

PixelOutput main(FullscreenVertexOutput input)
{
    PixelOutput result;
    
    float2 uv = input.position.xy / resolution.zw;
    float3 worldPosition = aWorldPositionTexture.Sample(aDefaultSampler, uv).rgb;
    
    float shadowFactor = 1.0f;
    {
        float4 wPos = float4(worldPosition.x, worldPosition.y, worldPosition.z, 1);
        float4 directionalLightProjectedPositionTemp = mul(directionalLightShadowSpaceToWorldSpace, wPos);
        float3 directionalLightProjectedPosition = directionalLightProjectedPositionTemp.xyz / directionalLightProjectedPositionTemp.w;
	
        if (clamp(directionalLightProjectedPosition.x, -1.0f, 1.0f) == directionalLightProjectedPosition.x &&
        clamp(directionalLightProjectedPosition.y, -1.0f, 1.0f) == directionalLightProjectedPosition.y)
        {
            const float computedZ = directionalLightProjectedPosition.z;
            const float bias = shadowBias;

            float totalFactor = 0.0f;

            // Filter kernel for [PCF] percentage-closer filtering eg. (3x3)
            const int numSamples = 9;
            // Offset scale decides how much the shadow edge is moved for "blurring"
            const float offsetScale = shadowOffsetScale;

            for (int i = -numSamples / 2; i <= numSamples / 2; ++i)
            {
                for (int j = -numSamples / 2; j <= numSamples / 2; ++j)
                {
                    const float2 sampleOffset = float2(i, j) / float(numSamples);
                    const float2 sampleUV = 0.5f + float2(0.5f, -0.5f) * (directionalLightProjectedPosition.xy + sampleOffset * offsetScale);
                    float depthValue = aDepthTexture.Sample(aDefaultSampler, sampleUV).r;
                    const float shadowMapZ = aShadowMapTexture.SampleCmpLevelZero(depthSamplerState, sampleUV, depthValue);
                    //const float shadowMapZ = aShadowMapTexture.Sample(aShadowSampler, sampleUV);
                
                    totalFactor += (computedZ < shadowMapZ + bias) ? 1.0f : 0.0f;
                }
            }

            shadowFactor = totalFactor / float(numSamples * numSamples);
        }
    }

    result.color = clamp(shadowFactor, useShadows, 1);
    
    return result;
}

