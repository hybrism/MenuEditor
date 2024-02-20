#include "../common/common.hlsli"

struct SpriteVertexInputType
{
    float4 position : POSITION;
    uint4 vertexIndex : TEXCOORD0;
    float4 instanceTransform0 : TEXCOORD1;
    float4 instanceTransform1 : TEXCOORD2;
    float4 instanceTransform2 : TEXCOORD3;
    float4 instanceTransform3 : TEXCOORD4;
    float4 instanceColor : TEXCOORD5;
    float4 instanceUV : TEXCOORD6;
    float4 uvRect : TEXCOORD7;
    float4 clipValue : TEXCOORD8;
};

//FROM TGE:
static uint2 textureRectLookup[6] =
{
    uint2(0, 1),
		uint2(0, 3),
		uint2(2, 1),
		uint2(2, 3),
		uint2(2, 1),
		uint2(0, 3),
};
	
float2 GetUVRect(float4 aRect, uint aIndex)
{
    uint2 theLookuped = textureRectLookup[aIndex];
    return float2(aRect[theLookuped.x], aRect[theLookuped.y]);
}

SpritePixelInputType main(SpriteVertexInputType input)
{
    SpritePixelInputType output;
        
    float4x4 transform = 0;
    transform._11_21_31_41 = float4(input.instanceTransform0.xyz, 0);
    transform._12_22_32_42 = float4(input.instanceTransform1.xyz, 0);
    transform._13_23_33_43 = float4(input.instanceTransform2.xyz, 0);
    transform._14_24_34_44 = float4(input.instanceTransform3.xyz, 1);

    float4 vertexObjectPos = mul(transform, input.position);
    float2 targetViewport = float2(resolution.z, resolution.w);
    
    float2 pos = (vertexObjectPos.xy / targetViewport.xy) * 2 - float2(1, 1);
    
    output.position = float4(pos.xy, 0, 1);
    
    float2 textureRect = GetUVRect(input.uvRect, input.vertexIndex.x);
    output.uv = input.instanceUV.xy + (textureRect * input.instanceUV.zw);
    
    output.color = input.instanceColor;
    output.clip = input.clipValue;
    
    return output;
}