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
    
    float3 cameraPosition = modelToWorld[3].xyz;
    float3 toEye = normalize(cameraPosition - input.worldPosition.xyz);
    
    
    float3 col = float3(0, 0, 0);
    
    float3 vLightDir = normalize(-float3(1, 1, 0));
    float3 vNormal = normalize(input.normal);
    float3 vEyeVec = normalize(toEye);
    
    float3 L = normalize(vLightDir);
    float3 N = normalize(vNormal);
    float3 E = normalize(vEyeVec);
    
      
    // Lambert Diffuse Lighting
    float diffuse = clamp(dot(N, -L), 0.0, 1.0);
    diffuse = smoothstep(0.0, 0.25, diffuse);
    //diffuse = lerp(0.05, 0.25, diffuse);
  
    // Blinn Specular
    float3 H = normalize(-L + E);
    float specular = pow(clamp(dot(N, H), 0.0, 1.0), 360.0);
    specular = smoothstep(0.0, 0.005, specular);
    specular = smoothstep(0.0, 0.005, specular);
    specular = 0;
    
    // Rim Lighting
    float rimlight = smoothstep(0.5, 0.05, dot(N, E));
    rimlight = smoothstep(0.0, 0.05, rimlight);
    
    float rimlight2 = smoothstep(0.35, 0.05, dot(N, E));
    rimlight2 = smoothstep(0.0, 0.05, rimlight2);
    
    float3 diffuseCol = float3(1, 0, 0);
    float3 lightColor = float3(0.1, 0.1, 0.1);

      // Add Rim Lighting to Diffuse Component
    float4 finalColor = float4(0, 0, 0, 1.0);
    finalColor.rgb = diffuseCol * diffuse;
    finalColor.rgb += (diffuseCol / 3.0) * (1.0 - diffuse);
    finalColor.rgb += lightColor / 3.0;
    finalColor.rgb += float3(specular, specular, specular);
    finalColor.rgb += float3(rimlight, rimlight, rimlight) * 0.5f * diffuse;
    finalColor.rgb += float3(rimlight2, rimlight2, rimlight2) * diffuse;
    
    result.color.rgba = finalColor;
    
    
	return result;
}