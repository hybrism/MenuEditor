#pragma once
#pragma message("-------Friendship is everything <3 (Game) !-------------")

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(NOMINMAX)
#define NOMINMAX
#endif

#if !defined(PX_PHYSICS_FOUNDATION_VERSION)
#define PX_PHYSICS_FOUNDATION_VERSION 0x05030100
#endif

#if !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

//ENGINE
#include <engine/Paths.h>
#include <engine/debug/DebugLine.h>
#include <engine/shaders/PixelShader.h>
#include <engine/shaders/VertexShader.h>
#include <engine/graphics/model/Mesh.h>
#include <engine/graphics/sprite/Sprite.h>
#include <engine/graphics/GraphicsEngine.h>
#include <engine/graphics/model/SkeletalMesh.h>
#include <engine/graphics/Animation/Animation.h>

#include <engine/math/Ray.h>
#include <engine/math/Vector.h>
#include <engine/math/helper.h>
#include <engine/math/Transform.h>
#include <engine/math/VectorFwd.h>

#include <engine/utility/Error.h>
#include <engine/utility/StringHelper.h>

//ASSET
#include <assets/AssetDatabase.h>
#include <assets/ShaderDatabase.h>

//ECS
#include <ecs/World.h>
#include <ecs/component/Component.h>
#include <ecs/entity/Entity.h>

//STD
#include <algorithm>
#include <array>
#include <cassert>
#include <exception>
#include <fstream>
#include <malloc.h>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>

//IMGUI
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>


//PHYSX inc
