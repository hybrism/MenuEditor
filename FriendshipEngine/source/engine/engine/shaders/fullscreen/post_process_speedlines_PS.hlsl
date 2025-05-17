#include "../common/common.hlsli"

cbuffer PostProcessBufferData : register(b5)
{
    PostProcessData postProcessData;
};

Texture2D postProcessTexture : register(t11);



float3 mod2D289(float3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
float2 mod2D289(float2 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
float3 permute(float3 x)
{
    return mod2D289(((x * 34.0) + 1.0) * x);
}
float snoise(float2 v)
{
    const float4 C = float4(0.211324865405187, 0.366025403784439, -0.577350269189626, 0.024390243902439);
    float2 i = floor(v + dot(v, C.yy));
    float2 x0 = v - i + dot(i, C.xx);
    float2 i1;
    i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
    float4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    i = mod2D289(i);
    float3 p = permute(permute(i.y + float3(0.0, i1.y, 1.0)) + i.x + float3(0.0, i1.x, 1.0));
    float3 m = max(0.5 - float3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0);
    m = m * m;
    m = m * m;
    float3 x = 2.0 * frac(p * C.www) - 1.0;
    float3 h = abs(x) - 0.5;
    float3 ox = floor(x + 0.5);
    float3 a0 = x - ox;
    m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);
    float3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}


float2 hash(float2 p) // replace this by something better
{
    p = float2(dot(p, float2(127.1, 311.7)), dot(p, float2(269.5, 183.3)));
    return -1.0 + 2.0 * frac(sin(p) * 43758.5453123);
}

float NoiseSimplex(in float2 p)
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

    float2 i = floor(p + (p.x + p.y) * K1);
    float2 a = p - i + (i.x + i.y) * K2;
    float m = step(a.y, a.x);
    float2 o = float2(m, 1.0 - m);
    float2 b = a - o + K2;
    float2 c = a - 1.0 + 2.0 * K2;
    float3 h = max(0.5 - float3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
    float3 n = h * h * h * h * float3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));
    return dot(n, float3(70.0, 70.0, 70.0));
}




#define PI 3.14159265359
#define PI_2 6.2831

float2 PolarCoordinates(float2 cent, float2 origo, float radialScale = 1, float tilingScale = 1, float rotationRadians = PI)
{
    return float2(atan2(cent.y, cent.x) * tilingScale + PI * rotationRadians, distance(cent, origo) * radialScale);
}

float InverseLerp(float a, float b, float value)
{
    return saturate((value - a) / (b - a));
}
float2 InverseLerp(float2 a, float2 b, float2 value)
{
    return saturate((value - a) / (b - a));
}
float3 InverseLerp(float3 a, float3 b, float3 value)
{
    return saturate((value - a) / (b - a));
}


float3 displayPolarPattern(float2 cent, float2 origo, float radialScale = 1, float tilingScale = 1, float rotationRadians = PI)
{
    float2 polar = PolarCoordinates(cent, origo, radialScale, tilingScale, rotationRadians);
    float col = lerp(
        lerp(0.0, 1.0, step(3.14, fmod(polar.x * 10.0, 6.28))),
        0.5,
        step(0.5, fmod(polar.y * 5.0, 1.0))
    );
    return float3(col, col, col);
}


float4 main(FullscreenVertexOutput input) : SV_Target
{
    float4 finalColor = 0;
    
    
    float animation = 3;
    float radialScale = 0.4;
    float radialTiling = 8;
    float linesPower = 1;
    float linesRemap = 0.8;
    
    float2 center = (input.uv.xy - 0.5) * 2.0;
    float2 origo = float2(0.0, 0.0);
    
    float2 polarCoords = PolarCoordinates(center, origo, radialScale, radialTiling);
    
    float add = -animation * time;
    float noise = NoiseSimplex(polarCoords + add);
    noise = pow(noise, linesPower);
    noise = saturate(noise);
    
    
    float radius = 15 + postProcessData.speedLineRadiusAddition;
    float edge = 0.15;
    float feather = 0.4f;
    
    float distanceFromCenter = clamp(length(center) / radius, 0.0, 1.0) * (noise);
    
    float falloffMask = 2.0 * distanceFromCenter - 1.0;
    falloffMask = 1.0 - pow(abs(falloffMask), 4.0);
    
    float thinnerMask = 2.0 * distanceFromCenter - 1.0;
    thinnerMask = pow(1.0 - abs(thinnerMask), 4.0);
    float steppedValue = smoothstep(edge, edge + feather, noise * falloffMask);
    
    finalColor = float4(postProcessTexture.Sample(aDefaultSampler, input.uv.xy).rgb, 1);
    finalColor.rgb += steppedValue; // * steppedValue;
    
    finalColor.a = 1.0f;
    
    return finalColor;
}