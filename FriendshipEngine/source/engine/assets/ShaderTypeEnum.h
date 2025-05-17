#pragma once

enum class VsType
{
	DebugLine3D,
	DebugLine2D,

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
	Particle,

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
	Speedlines,

	// sprite
	Sprite,

	// common
	MissingTextureDeferred,
	MissingTextureLegacy,
	DebugLine,
	DebugLightSphere,

	// vfx
	Aoe1,
	Aoe2,
	Aoe3,

	//Light
	DirectionalLight,
	LightBounds,
	SkyBox,

	Particle,

	Count
};



