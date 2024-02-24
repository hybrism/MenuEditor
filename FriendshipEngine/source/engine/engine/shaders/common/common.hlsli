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
    uint1 index : SV_VertexID;
};

struct AnimatedVertexInputType : VertexInputType
{
    uint4 boneIndices : BONES; // only 4 bones per vertex supported for now, expand if needed
    float4 weights : WEIGHTS;
};

struct VertexInstancedInputType : VertexInputType
{
    float4x4 instanceTransform : WORLD;
    //uint InstanceID : SV_InstanceID;
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
    float4 directionalLightsDirection;
    float3 directionalLightsColor;
    float directionalLightsIntensity;
    float4x4 directionalLightShadowSpaceToWorldSpace;

    struct PointLight
    {
        float3 pointLightsPosition;
        float pointLightsRange;
        float3 pointLightsColor;
        float pointLightsIntensity;
    } myPointLights[32];

    int amountOfPointLights;
    float unused[3];
};




SamplerState aDefaultSampler : register(s0);
SamplerState a2dDefaultSampler : register(s1);
SamplerState aClampingSampler : register(s2);


