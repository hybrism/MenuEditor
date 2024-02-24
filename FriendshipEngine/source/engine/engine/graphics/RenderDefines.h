#pragma once

enum ShaderDataID
{
    ShaderDataID_1,
    ShaderDataID_2,
    ShaderDataID_3,
    ShaderDataID_4,
    ShaderDataID_5,
    ShaderDataID_6,
    ShaderDataID_7,
    ShaderDataID_8,
};

enum class ShaderTextureSlot : int
{
    Cubemap = 0,
    Albedo = 1,
	Normal = 2,
	VertexNormal = 3,
    Material = 4,
    Emissive = 5,
    Slot5 = 6, //TGE: Below 4 is the standard texture slots, keep above it! (: register( t4 ); in the shader)
	Slot6 = 7,  //"enum class GBufferTexture", in "GBuffer.h" uses enum slots 0 - 6
    ShadowMap = 8,
	DirectionalLight = 9,
	LightBound = 10,
};

enum class ShaderDataBufferIndex
{
    Index_0 = 1,
    Index_1 = 5, // Below 5 is the standard buffer slots, keep above it! (: register(b5) in shader)
    Index_2 = 6,
    Index_3 = 7,
    Index_4 = 8,
};

enum class RenderMode
{
	POINTLIST = 1,
	LINELIST = 2,
	TRIANGLELIST = 4,
};

enum class BufferSlots : int
{
	Frame,
	Object,
	Bone,
	Input,
	Light,
	PostProcess
};

enum class DepthStencilState
{
	ReadWrite,
	ReadOnly,
	ReadOnlyGreater,
	Additive, //???
	Disabled,
	Count
};

enum class RasterizerState
{
	BackfaceCulling,
	FrontfaceCulling,
	NoFaceCulling,
	Wireframe,
	WireframeNoCulling,
	Count,
};

enum class BlendState : int
{
	Disabled = 0,
	AlphaBlend,
	AdditiveBlend,
	Count
};

class Texture;

struct BoundTexture
{
	BoundTexture() {}
	BoundTexture(Texture* aTex, unsigned char aIndex)
	{
		myTexture = aTex;
		myIndex = aIndex;
	}
	Texture* myTexture = nullptr;
	unsigned char myIndex = 0;
};

struct RenderState
{
	DepthStencilState depthStencilState = DepthStencilState::ReadWrite;
	RasterizerState rasterizerState = RasterizerState::BackfaceCulling;
	BlendState blendState = BlendState::Disabled;
};
