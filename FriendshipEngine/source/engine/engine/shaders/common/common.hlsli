#include "constants.hlsli"

#define MAX_ANIMATION_BONES 64 
#define MAX_NUMBER_OF_LIGHTS 8
#define MAX_POINTLIGHTS 32

int GetNumMips(TextureCube cubeTex)
{
    int iWidth = 0;
    int iheight = 0;
    int numMips = 0;
    cubeTex.GetDimensions(0, iWidth, iheight, numMips);
    return numMips;
}

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 binormal : BINORMAL;
    float3 tangent : TANGENT;
    uint1 vertexIndex : SV_VertexID;
};

float ui2f8(uint value)
{
    return float(value) / 255.0f;
}

// Decode floating-point value back to 8-bit integer value
uint f2ui8(float value)
{
    return uint(value * 255.0f);
}

uint f42ui32(float4 value)
{
    //return (f2ui8(value.x) << 24) | (f2ui8(value.y) << 16) | (f2ui8(value.z) << 8) | f2ui8(value.w);
    return (f2ui8(value.w) << 24) | (f2ui8(value.z) << 16) | (f2ui8(value.y) << 8) | f2ui8(value.a);
}

uint EffectToEntityID(float4 effects)
{
    uint entityId = asuint(effects.a) | (asuint(effects.b) << 8);
    entityId = entityId & 0xFFFF; // only keep lower 16 bits
    return entityId;
}

float2 EntityIDToEffect(uint entityId)
{
    float2 effects;
    effects.x = ui2f8((entityId >> 8) & 0xFF);
    effects.y = ui2f8(entityId & 0xFF);
    return effects;
}

struct AnimatedVertexInputType : VertexInputType
{
    uint4 boneIndices : BONES; // only 4 bones per vertex supported for now, expand if needed
    float4 weights : WEIGHTS;
};

struct VertexInstancedInputType : VertexInputType
{
    float4x4 instanceTransform : WORLD;
    uint2 entityData : ENTITY; // x = entityId, y = render order
};

struct AnimatedVertexInstancedInputType : AnimatedVertexInputType
{
    float4x4 instanceTransform : WORLD;
    //uint InstanceID : SV_InstanceID;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 binormal : BINORMAL;
    float3 tangent : TANGENT;
};

struct DeferredPixelInputType : PixelInputType
{
    uint2 entityData : ENTITY; // x = entityId, y = render order
};

struct SpritePixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float4 clip : TEXCOORD1;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};

struct FullscreenVertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

struct GBufferOutput
{
    float4 worldPosition : SV_TARGET0;
    float4 albedo : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 vertexNormal : SV_TARGET3;
    float4 material : SV_TARGET4;
    float4 effects : SV_TARGET5; // use other format with the new texture packing????
};

struct PostProcessData
{
    int downScaleLevel;
    float alphaBlendLevel;
    float saturation;
    float exposure;
    
    float contrast;
    float vignetteInner;
    float vignetteOuter;
    float vignetteStrength;
    
    float3 tint;
    float vignetteCurvature;
    
    float3 blackPoint;
    float3 vignetteColor;
    
    float2 trash;
};

// TODO: separate resolution and viewport
cbuffer FrameBuffer : register(b0)
{
    float4 resolution; //x = screen width, y = screen height, z = viewport width, w = viewport height
    float4x4 worldToClipMatrix; //ProjectionMatrix
    float4x4 modelToWorld; //WorldToCamera, ModelMatrix, ViewModel, ViewMatrix
    float NearPlane;
    float FarPlane;
    float2 unused0;
}

cbuffer ObjectBuffer : register(b1)
{
    float4x4 objectTransform;
}

cbuffer BoneBuffer : register(b2)
{
    float4x4 bones[MAX_ANIMATION_BONES];
};

cbuffer InputBuffer : register(b3)
{
    float1 time;
    float1 deltaTime;
    uint1 renderMode;
    float1 trash;
}

cbuffer LightBufferData : register(b4)
{
    float3 directionalLightsDirection;
    float directionalLightsIntensity;
    float3 directionalLightsColor;
    float ambientLightIntensity;
    float4x4 directionalLightShadowSpaceToWorldSpace;

    struct PointLight
    {
        float3 pointLightsPosition;
        float pointLightsRange;
        float3 pointLightsColor;
        float pointLightsIntensity;
    } myPointLights[32];

    int amountOfPointLights;
    
     int useShadows;
    
    float unused[2];
};

cbuffer PointLightSphereColorBufferData : register(b5)
{
    float4 lightSphereColor;
}


float4x4 Fastinverse(float4x4 mat)
{
    float4 c0 = mat[0];
    float4 c1 = mat[1];
    float4 c2 = mat[2];
    float4 c3 = mat[3];

    float4 r0 = float4(c0.x, c1.x, c2.x, 0.0f);
    float4 r1 = float4(c0.y, c1.y, c2.y, 0.0f);
    float4 r2 = float4(c0.z, c1.z, c2.z, 0.0f);
    float4 r3 = float4(0.0f, 0.0f, 0.0f, 1.0f);

    float4 det0 = float4(c1.y * c2.z - c1.z * c2.y, c0.z * c2.y - c0.y * c2.z, c0.y * c1.z - c0.z * c1.y, 0.0f);
    float4 det1 = float4(c1.x * c2.z - c1.z * c2.x, c0.z * c2.x - c0.x * c2.z, c0.x * c1.z - c0.z * c1.x, 0.0f);
    float4 det2 = float4(c1.x * c2.y - c1.y * c2.x, c0.y * c2.x - c0.x * c2.y, c0.x * c1.y - c0.y * c1.x, 0.0f);

    float4 invDet = 1.0f / dot(det0, c0);

    r0 *= invDet;
    r1 *= invDet;
    r2 *= invDet;
    r3 *= invDet;

    return float4x4(r0, r1, r2, r3);
}

float4x4 inverse(float4x4 m)
{
    float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
    float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
    float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
    float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

    float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
    float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
    float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
    float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
    float idet = 1.0f / det;

    float4x4 ret;

    ret[0][0] = t11 * idet;
    ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
    ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
    ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

    ret[1][0] = t12 * idet;
    ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
    ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
    ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

    ret[2][0] = t13 * idet;
    ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
    ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
    ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

    ret[3][0] = t14 * idet;
    ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
    ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
    ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

    return ret;
}

float3 UnpackNormal(float3 normalTexture)
{
    return normalize(2.0f * normalTexture - 1.0f);
}

float3 GetCameraPosition()
{
    float4x4 mat = inverse(modelToWorld);
    return float3(mat[0].w, mat[1].w, mat[2].w);
}

SamplerState aDefaultSampler : register(s0);
SamplerState aClampingSampler : register(s1);


