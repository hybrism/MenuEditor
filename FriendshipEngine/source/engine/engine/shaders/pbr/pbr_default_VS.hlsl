#include "../common/common.hlsli"

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

    float4 vertexObjectPos = mul(objectTransform, float4(input.position.xyz, 1));
    float4 vertexViewPos = mul(modelToWorld, float4(vertexObjectPos.xyz, 1.0f));
    float4 vertexClipPos = mul(worldToClipMatrix, float4(vertexViewPos.xyz, 1.0f));
	
	output.color = float4(input.color.xyz, 1.0f);
    output.binormal = input.binormal;
    output.normal = input.normal;
    output.tangent = input.tangent;
    output.uv = input.uv;
    output.position = vertexClipPos;
    output.worldPosition = vertexObjectPos;
    
	return output;
}
