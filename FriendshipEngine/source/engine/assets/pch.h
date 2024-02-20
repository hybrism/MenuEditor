#pragma once
#pragma message("-------Friendship is everything <3 (Assets) !-------------")

//External
#include <DirectXMath.h>
#include <fbxsdk.h>

//Engine
#include <engine/graphics/GraphicsEngine.h>
#include <engine/graphics/model/Mesh.h>
#include <engine/graphics/model/SkeletalMesh.h>
#include <engine/graphics/animation/Skeleton.h>
#include <engine/Paths.h>
#include <engine/utility/StringHelper.h>
#include <engine/utility/Error.h>
#include <engine/graphics/animation/Animation.h>
#include <engine/graphics/animation/Bone.h>

//STD
#include <algorithm>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <memory>
#include <new>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>

//Assets
#include "AssetDefines.h"
#include "FactoryStructs.h"
#include "FBXLoadStructs.h"
#include "SharedFactoryFunctions.h"
