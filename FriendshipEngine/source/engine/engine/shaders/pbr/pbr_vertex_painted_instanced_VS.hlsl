#include "../common/common.hlsli"

Texture2D aVertexColorTexture : register(t0);

uint2 GetUVIndex(uint index, uint width, uint height)
{
    float4 vertexColors = aVertexColorTexture.Load(int3(0, 0, 0));
    
    uint vertCount = f2ui8(vertexColors.r);
    vertCount |= f2ui8(vertexColors.g) << 8;
    vertCount |= f2ui8(vertexColors.b) << 16;
    vertCount |= f2ui8(vertexColors.a) << 24;
    
    uint i = vertCount * index + 1;
    return uint2(i % width, i / height);
}

DeferredPixelInputType main(VertexInstancedInputType input)
{
    DeferredPixelInputType output;

    float4 vertexObjectPos = mul(input.instanceTransform, float4(input.position.xyz, 1));
    float4 vertexViewPos = mul(modelToWorld, float4(vertexObjectPos.xyz, 1.0f));
    float4 vertexClipPos = mul(worldToClipMatrix, float4(vertexViewPos.xyz, 1.0f));
	
    
    const float3x3 objectToWorldRotation = float3x3(input.instanceTransform._11, input.instanceTransform._12, input.instanceTransform._13,
                                                    input.instanceTransform._21, input.instanceTransform._22, input.instanceTransform._23,
                                                    input.instanceTransform._31, input.instanceTransform._32, input.instanceTransform._33);
    
    output.color = float4(input.color.xyz, 1.0f);
    output.binormal = mul(objectToWorldRotation, input.binormal);
    output.normal = mul(objectToWorldRotation, input.normal);
    output.tangent = mul(objectToWorldRotation, input.tangent);
    output.uv = input.uv;
    output.position = vertexClipPos;
    output.worldPosition = vertexObjectPos;
    output.entityData = input.entityData;
    //output.color = aVertexColorTexture.Sample(aDefaultSampler, input.uv, int2(0, 0));
    
    
    
    uint width = 0;
    uint height = 0;
    uint levels = 0;
    aVertexColorTexture.GetDimensions(0, width, height, levels);
    
    uint2 startUvLocation = GetUVIndex(input.entityData.y, width, height);
    
    uint index = input.vertexIndex;
    uint2 uvLocation = startUvLocation;
    uvLocation.y += uint(index / height);
    uvLocation.x += uint(index % width);

    //output.color = test[uvLocation.y][uvLocation.x];
    output.color = aVertexColorTexture.Load(int3(uvLocation.x, uvLocation.y, 0));
    //output.color = aVertexColorTexture.Load(int3(1, 1, 0));
    
    
    //output.color = aVertexColorTexture.Sample(samLinear, float2(0, 0), int2(0, 0));
    
    //uint size = 8;
    //float4 test[8][8];
    //test[0][0] = float4(GetUVIndex(0), 0, 0);
    //test[0][1] = float4(GetUVIndex(1), 0, 0);
    //test[0][2] = float4(GetUVIndex(2), 0, 0);
    //test[0][3] = float4(GetUVIndex(3), 0, 0);

    //test[1][0] = float4(1, 0, 0, 1);
    //test[1][1] = float4(0, 1, 0, 1);
    //test[1][2] = float4(0, 0, 1, 1);
    //test[1][3] = float4(1, 1, 0, 1);
    //test[1][4] = float4(1, 0, 1, 1);
    //test[1][5] = float4(0, 1, 1, 1);
    //test[1][6] = float4(0, 0, 0, 1);
    //test[1][7] = float4(1, 0, 1, 1);

    //test[3][0] = float4(0.5f, 0.5f, 0.5f, 1);
    //test[3][1] = float4(0.5f, 0.5f, 0.5f, 1);
    //test[3][2] = float4(0.5f, 0.5f, 0.5f, 1);
    //test[3][3] = float4(0.5f, 0.5f, 0.5f, 1);

    //output.color = float4(0, 0, 0, 1);
    ////output.color = test[input.vertexIndex][0];
    //uint index = input.vertexIndex;

    //uint entityId = 0;
    //uint2 startUvLocation = GetUVIndex(input.entityData.y);

    //uint2 uvLocation = startUvLocation;
    //uvLocation.y += uint(index / size);
    //uvLocation.x += uint(index % size);

    //output.color = test[uvLocation.y][uvLocation.x];
    //output.color.a = 1;
    
	return output;
}
