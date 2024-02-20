#pragma once

enum class VsType
{
	// pbr
	DefaultPBR,
	SkinnedPBR,
	DefaultPBRInstanced,

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

	// vfx
	TestVFX,

	//Light
	DirectionalLight,
	LightBounds,

	Count
};



