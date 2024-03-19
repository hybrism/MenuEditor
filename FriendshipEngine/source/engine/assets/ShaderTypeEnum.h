#pragma once

enum class VsType
{
	// pbr
	DefaultPBR,
	SkinnedPBR,
	DefaultPBRInstanced,
	VertexPaintedPBRInstanced,

	// fullscreen
	Fullscreen,

	// sprite
	InstancedSprite,
	Instanced3DSprite,

	// vfx
	DefaultVFX,

	Count
};

enum class PsType
{
	// pbr
	DefaultPBR,
	SilhouettePBR,
	DeferredPBR,
	GBuffer,
	
	// fullscreen
	Fullscreen,
	PostProcess,
	ToneMap,
	Vignette,

	// sprite
	Sprite,

	// common
	MissingTextureDeferred,
	MissingTextureLegacy,
	DebugLine,
	DebugLightSphere,

	// vfx
	TestVFX,

	//Light
	DirectionalLight,
	LightBounds,
	SkyBox,

	Count
};



