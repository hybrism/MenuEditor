#include "../common/common.hlsli"

struct VfxVertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 binormal : BINORMAL;
    float3 tangent : TANGENT;
    float1 time : TEXCOORD1;
};

struct VfxPixelInputType
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 binormal : BINORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD0;
    float1 time : TEXCOORD1;
};
