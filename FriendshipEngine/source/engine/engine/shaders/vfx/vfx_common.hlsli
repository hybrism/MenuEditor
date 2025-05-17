#include "../common/common.hlsli"

struct VfxVertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 binormal : BINORMAL;
    float3 tangent : TANGENT;
    uint1 vertexIndex : SV_VertexID;
    float4x4 instanceTransform : WORLD;
    float time : TIME;
};

struct VfxPixelInputType
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 binormal : BINORMAL;
    float3 tangent : TANGENT;
    //float1 time : TEXCOORD1;
    //float1 time : TIME;
    float time : TIME;
    float depth : DEPTH;
};

struct ParticleVertexInputType
{
    float4 position : POSITION;
    float4 vertexColor : COLOR0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 binormal : BINORMAL;
    float3 tangent : TANGENT;
    float4x4 instanceTransform : WORLD;
    float4 color : COLOR1;
    float time : TIME;
};

struct ParticlePixelInputType
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float depth : DEPTH;
    float time : TIME;
};
