#pragma once

//TODO: Fix assetpaths!
#ifndef _RELEASE

static constexpr const char* RELATIVE_ASSET_PATH = "../../../P7_Grupp4_Unity/Assets/Resources/";
static constexpr const char* RELATIVE_SPRITE_ASSET_PATH = "../../content/sprites/";
static constexpr const char* RELATIVE_AUDIO_ASSET_PATH = "../../content/audio/";
static constexpr const char* RELATIVE_FMOD_ASSET_PATH = "../../content/audio/Desktop/";
static constexpr const char* RELATIVE_FONT_ASSET_PATH = "../../content/fonts/";
static constexpr const char* RELATIVE_CUBEMAP_ASSET_PATH = "../../content/cubemap/";
static constexpr const char* RELATIVE_SHADER_PATH = "../../content/shaders/";
static constexpr const char* RELATIVE_IMPORT_PATH = "../../import/";
static constexpr const char* RELATIVE_IMPORT_DATA_PATH = "../../import/data/";
static constexpr const char* RELATIVE_CUSTOM_MESH_DATA_PATH = "../../content/models/";
static constexpr const char* RELATIVE_CUSTOM_ANIMATION_DATA_PATH = "../../content/animations/";
static constexpr const char* RELATIVE_CUSTOM_ANIMATION_RELEASE_DATA_PATH = "../release/animations/";
static constexpr const char* RELATIVE_MODELVIEWER_ASSET_PATH = "../../content/modelViewer/";
static constexpr const char* RELATIVE_EDITOR_ASSET_PATH = "../../content/editor/";
static constexpr const char* RELATIVE_VERTEX_TEXTURE_ASSET_PATH = "../../content/vertexTextures/";

#else

static constexpr const char* RELATIVE_ASSET_PATH = "./content/";
static constexpr const char* RELATIVE_SPRITE_ASSET_PATH = "./content/sprites/";
static constexpr const char* RELATIVE_AUDIO_ASSET_PATH = "./content/audio/";
static constexpr const char* RELATIVE_FMOD_ASSET_PATH = "./content/audio/Desktop/";
static constexpr const char* RELATIVE_FONT_ASSET_PATH = "./content/fonts/";
static constexpr const char* RELATIVE_CUBEMAP_ASSET_PATH = "./content/cubemap/";
static constexpr const char* RELATIVE_SHADER_PATH = "./content/shaders/";
static constexpr const char* RELATIVE_IMPORT_PATH = "./import/";
static constexpr const char* RELATIVE_IMPORT_DATA_PATH = "./import/data/";
static constexpr const char* RELATIVE_CUSTOM_MESH_DATA_PATH = "./content/models/";
static constexpr const char* RELATIVE_CUSTOM_ANIMATION_DATA_PATH = "./content/animations/";
static constexpr const char* RELATIVE_MODELVIEWER_ASSET_PATH = "./content/modelViewer/";
static constexpr const char* RELATIVE_EDITOR_ASSET_PATH = "./content/editor/";
static constexpr const char* RELATIVE_VERTEX_TEXTURE_ASSET_PATH = "./content/vertexTextures/";


//static constexpr const char* RELATIVE_ASSET_PATH = "./content/";
//static constexpr const char* RELATIVE_SPRITE_ASSET_PATH = "./content/sprites/";
//static constexpr const char* RELATIVE_AUDIO_ASSET_PATH = "./content/audio/";
//static constexpr const char* RELATIVE_FMOD_ASSET_PATH = "./content/audio/Desktop/";
//static constexpr const char* RELATIVE_FONT_ASSET_PATH = "./content/fonts/";
//static constexpr const char* RELATIVE_CUBEMAP_ASSET_PATH = "./content/cubemap/";
//static constexpr const char* RELATIVE_SHADER_PATH = "./shaders/";
//static constexpr const char* RELATIVE_IMPORT_PATH = "./import/";
//static constexpr const char* RELATIVE_IMPORT_DATA_PATH = "./import/data/";
//static constexpr const char* RELATIVE_CUSTOM_MESH_DATA_PATH = "./models/";
//static constexpr const char* RELATIVE_CUSTOM_ANIMATION_DATA_PATH = "./animations/";
//static constexpr const char* RELATIVE_VERTEX_TEXTURE_ASSET_PATH = "./content/vertexTextures/";
#endif
 

