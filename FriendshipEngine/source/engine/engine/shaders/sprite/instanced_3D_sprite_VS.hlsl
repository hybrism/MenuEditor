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

float3x3 inverse3x3(float3x3 mat)
{
    float det = mat._m00 * (mat._m11 * mat._m22 - mat._m21 * mat._m12) -
                mat._m01 * (mat._m10 * mat._m22 - mat._m20 * mat._m12) +
                mat._m02 * (mat._m10 * mat._m21 - mat._m20 * mat._m11);

    float invDet = 1.0 / det;

    float3x3 result;
    result._m00 = (mat._m11 * mat._m22 - mat._m21 * mat._m12) * invDet;
    result._m01 = -(mat._m01 * mat._m22 - mat._m21 * mat._m02) * invDet;
    result._m02 = (mat._m01 * mat._m12 - mat._m11 * mat._m02) * invDet;

    result._m10 = -(mat._m10 * mat._m22 - mat._m20 * mat._m12) * invDet;
    result._m11 = (mat._m00 * mat._m22 - mat._m20 * mat._m02) * invDet;
    result._m12 = -(mat._m00 * mat._m12 - mat._m10 * mat._m02) * invDet;

    result._m20 = (mat._m10 * mat._m21 - mat._m20 * mat._m11) * invDet;
    result._m21 = -(mat._m00 * mat._m21 - mat._m20 * mat._m01) * invDet;
    result._m22 = (mat._m00 * mat._m11 - mat._m10 * mat._m01) * invDet;

    return result;
}

float4x4 RotateTowardsCamera(float4x4 originalMatrix)
{
    // Extract the camera's rotation from the view matrix
    float3x3 cameraRotation = (float3x3) modelToWorld;
    cameraRotation = inverse3x3(cameraRotation);
    
    cameraRotation = mul(
        cameraRotation, 
        float3x3(
            originalMatrix._11_21_31,
            originalMatrix._12_22_32,
            originalMatrix._13_23_33
        )
    );
    
    return float4x4(
        float4(cameraRotation[0], 0),
        float4(cameraRotation[1], 0),
        float4(cameraRotation[2], 0),
        0, 0, 0, 1
    );
}

SpritePixelInputType main(SpriteVertexInputType input)
{
    SpritePixelInputType output;
       
    float4x4 transform = 0;
    transform._11_21_31_41 = float4(input.instanceTransform0.xy, 0, 0);
    transform._12_22_32_42 = float4(input.instanceTransform1.xy, 0, 0);
    transform._13_23_33_43 = float4(0, 0, 1, 0);
    transform._14_24_34_44 = float4(input.instanceTransform3.xyz, 1);
        
    
    float4 vertexObjectPos = mul(RotateTowardsCamera(transform), float4(input.position.xyz, 1));
    vertexObjectPos.xyz += input.instanceTransform3.xyz;
    float4 vertexViewPos = mul(modelToWorld, float4(vertexObjectPos.xyz, 1.0f));
    float4 vertexClipPos = mul(worldToClipMatrix, float4(vertexViewPos.xyz, 1.0f));
    
    output.position = vertexClipPos;
    
    
    
    //float4x4 transform = 0;
    //transform._11_21_31_41 = float4(input.instanceTransform0.xyz, 0);
    //transform._12_22_32_42 = float4(input.instanceTransform1.xyz, 0);
    //transform._13_23_33_43 = float4(input.instanceTransform2.xyz, 0);
    //transform._14_24_34_44 = float4(0, 0, 0, 1);

    //float4 vertexObjectPos = mul(transform, input.position);
    //float2 targetViewport = float2(resolution.z, resolution.w);
    
    //float2 pos = (vertexObjectPos.xy / targetViewport.xy) * 2 - float2(1, 1);
    
    
    //float4 vertexViewPos = mul(modelToWorld, float4(input.instanceTransform3.xyz, 1.0f));
    //float4 vertexClipPos = mul(worldToClipMatrix, float4(vertexViewPos.xyz, 1.0f));
    
    //output.position = float4(pos.xy, 0, 1) + vertexClipPos;
    
    
    

    //WE DON'T USE THE NORMAL IN PIXELSHADER RN, but can be used later
    //float3x3 toWorldRotation = (float3x3) transform;
    //output.normal = mul(float3(0.f, 0.f, -1.f), toWorldRotation);
        
    float2 textureRect = GetUVRect(input.uvRect, input.vertexIndex.x);
    
    //output.position = vertexObjectPos;
    output.uv = input.instanceUV.xy + (textureRect * input.instanceUV.zw);
    output.color = input.instanceColor;
    output.clip = input.clipValue;
    
    return output;
}