#include "common.hlsli"

PixelOutput main(PixelInputType input)
{
	PixelOutput result;
    float4 vertexObjectNormal = float4(input.normal, 0.0);
    float4 vertexWorldNormal = mul(modelToWorld, vertexObjectNormal);

    result.color = float4(1, 0, 1, 1);
    
    float lightIntensity = dot(vertexWorldNormal, float4(0, 1, 0, 0)) * 0.5 + 0.5;
    result.color = saturate(float4(1, 0, 1, 1) * lightIntensity);
    result.color.a = 1.f;
	return result;
}