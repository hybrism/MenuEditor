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
};

SpritePixelInputType main(SpriteVertexInputType input)
{
    SpritePixelInputType output;

    float2 pos = input.position.xy / resolution.zw;
    
    output.position = float4(pos.xy, 0, 1);
  
    output.color = float4(input.instanceColor.rgba);
    
    output.uv = input.instanceUV;
   
    return output;
}
