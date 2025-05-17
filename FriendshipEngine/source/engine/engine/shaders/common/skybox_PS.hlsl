#include "common.hlsli"

TextureCube SkyBox : register(t13);


PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    
    SkyBox.Sample(aDefaultSampler, (1, 1, 1));
    
    float3 campPos = GetCameraPosition().xyz; //modelToWorld._41_42_43;
    float3 wPos = input.worldPosition.xyz;
    
    float3 dir = normalize(wPos - campPos);
    
    
    result.color.rgb = SkyBox.SampleLevel(aDefaultSampler, dir, 0).rgb;
	
    return result;
}