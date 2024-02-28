#pragma once

//TODO: Fix assetpaths!
#ifdef _DEBUG

static constexpr char* RELATIVE_ASSET_PATH = "../../../P7_Grupp4_Unity/Assets/Resources/";
static constexpr char* RELATIVE_SPRITE_ASSET_PATH = "../../content/sprites/";
static constexpr char* RELATIVE_AUDIO_ASSET_PATH = "../../content/audio/";
static constexpr char* RELATIVE_FMOD_ASSET_PATH = "../../content/audio/Desktop/";
static constexpr char* RELATIVE_FONT_ASSET_PATH = "../../content/fonts/";
static constexpr char* RELATIVE_CUBEMAP_ASSET_PATH = "../../content/cubemap/";
static constexpr char* RELATIVE_SHADER_PATH = "../shaders/";
static constexpr char* RELATIVE_IMPORT_PATH = "../../import/";
static constexpr char* RELATIVE_IMPORT_DATA_PATH = "../../import/data/";
static constexpr char* RELATIVE_CUSTOM_MESH_DATA_PATH = "./models/";
static constexpr char* RELATIVE_CUSTOM_MESH_DATA_RELEASE_PATH = "../release/models/";
static constexpr char* RELATIVE_CUSTOM_ANIMATION_DATA_PATH = "./animations/";
static constexpr char* RELATIVE_CUSTOM_ANIMATION_RELEASE_DATA_PATH = "../release/animations/";
static constexpr char* RELATIVE_MODELVIEWER_ASSET_PATH = "../../content/modelViewer/";
static constexpr char* RELATIVE_EDITOR_ASSET_PATH = "../../content/editor/";

#else

static constexpr char* RELATIVE_ASSET_PATH = "../../../P7_Grupp4_Unity/Assets/Resources/";
static constexpr char* RELATIVE_SPRITE_ASSET_PATH = "Sprites/";
static constexpr char* RELATIVE_AUDIO_ASSET_PATH = "../../content/audio/";
static constexpr char* RELATIVE_FMOD_ASSET_PATH = "../../content/audio/Desktop/";
static constexpr char* RELATIVE_FONT_ASSET_PATH = "../../content/fonts/";
static constexpr char* RELATIVE_CUBEMAP_ASSET_PATH = "../../content/cubemap/";
static constexpr char* RELATIVE_SHADER_PATH = "../shaders/";
static constexpr char* RELATIVE_IMPORT_PATH = "../../import/";
static constexpr char* RELATIVE_IMPORT_DATA_PATH = "../../import/data/";
static constexpr char* RELATIVE_CUSTOM_MESH_DATA_PATH = "./models/";
static constexpr char* RELATIVE_CUSTOM_ANIMATION_DATA_PATH = "./animations/";
static constexpr char* RELATIVE_MODELVIEWER_ASSET_PATH = "../../content/modelViewer/";
static constexpr char* RELATIVE_EDITOR_ASSET_PATH = "../../content/editor/";



//static constexpr char* RELATIVE_ASSET_PATH = "./content/";
//static constexpr char* RELATIVE_SPRITE_ASSET_PATH = "Sprites/";
//static constexpr char* RELATIVE_FONT_ASSET_PATH = "./content/fonts/";
//static constexpr char* RELATIVE_AUDIO_ASSET_PATH = "./content/audio/";
//static constexpr char* RELATIVE_CUBEMAP_ASSET_PATH = "./content/cubemap/";
//static constexpr char* RELATIVE_SHADER_PATH = "./shaders/";
//static constexpr char* RELATIVE_IMPORT_PATH = "./import/";
//static constexpr char* RELATIVE_IMPORT_DATA_PATH = "./import/data/";
//static constexpr char* RELATIVE_CUSTOM_MESH_DATA_PATH = "./models/";
//static constexpr char* RELATIVE_CUSTOM_ANIMATION_DATA_PATH = "./animations/";
#endif
 

