#include "../common/common.hlsli"

Texture2D aVertexColorTexture : register(t0);

uint2 GetInitialUVIndexFromEntity(uint entityIndex, uint width, uint height)
{
    float4 vertexColors = aVertexColorTexture.Load(int3(0, 0, 0));
    
    uint vertCount = f2ui8(vertexColors.r);
    vertCount |= f2ui8(vertexColors.g) << 8;
    vertCount |= f2ui8(vertexColors.b) << 16;
    vertCount |= f2ui8(vertexColors.a) << 24;
    
    uint step = vertCount * entityIndex + 1;
    return uint2(step % width, step / height);
}

uint2 GetUVIndexFromEntityVertex(uint vertexIndex, uint entityIndex)
{
    uint width = 0;
    uint height = 0;
    uint levels = 0;
    aVertexColorTexture.GetDimensions(0, width, height, levels);
    
    uint2 startUvLocation = GetInitialUVIndexFromEntity(entityIndex, width, height);
    
    uint2 result = startUvLocation;
    result.x += vertexIndex;
    result.y += result.x / height;
    result.x %= width;
    
    return result;
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
    
    output.binormal = mul(objectToWorldRotation, input.binormal);
    output.normal = mul(objectToWorldRotation, input.normal);
    output.tangent = mul(objectToWorldRotation, input.tangent);
    output.uv = input.uv;
    output.position = vertexClipPos;
    output.worldPosition = vertexObjectPos;
    output.entityData = input.entityData;
    output.color = float4(0, 0, 0, 0);
    
    if (input.entityData.y >= 0)
    {
        uint2 uvLocation = GetUVIndexFromEntityVertex(input.vertexIndex, input.entityData.y);
        output.color = aVertexColorTexture.Load(int3(uvLocation.x, uvLocation.y, 0));
    }
    
    return output;
}
