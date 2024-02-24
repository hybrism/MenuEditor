project "Game"
	location (dirs.game_core)
	dependson { "external", "engine", "ecs", "assets" }
	
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	debugdir "%{dirs.bin}%{cfg.buildcfg}"
	targetdir ("%{dirs.bin}%{cfg.buildcfg}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")

	links {"external", "engine", "ecs", "assets" }

	pchheader "pch.h"
	pchsource ("pch.cpp")

	includedirs { ".", dirs.external, dirs.engine }

	files {
		"**.h",
		"**.cpp",
		"**.hlsl",
        "**.hlsli",
	}

	links{
		"PhysX_64.lib",
		"PhysXCommon_64.lib", 
		"PhysXFoundation_64.lib", 
		"PVDRuntime_64.lib", 
		"PhysXExtensions_static_64.lib", 
		"PhysXPvdSDK_static_64.lib", 
		"PhysXCooking_64.lib",
		"PhysXCharacterKinematic_static_64.lib",
		"fmod_vc.lib",
		"fmodstudio_vc.lib"
	}
	--verify_or_create_settings("Game")
	
	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
		libdirs { dirs.dependencies_debug }	
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
		libdirs { dirs.dependencies_release }	
	filter "system:windows"
--		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		--conformanceMode "On"
		--buildoptions { "/permissive" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			"FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		
		defines {
			"WIN32",
			"_LIB"
		}

	shadermodel("5.0")
	os.mkdir(dirs.shaders)
	filter("files:**.hlsl")
		flags("ExcludeFromBuild")
			shaderobjectfileoutput(dirs.shaders .. "%{file.basename}.cso")

    filter("files:**PS.hlsl")
        removeflags("ExcludeFromBuild")
        shadertype("Pixel")

    filter("files:**VS.hlsl")
        removeflags("ExcludeFromBuild")
        shadertype("Vertex")

    filter("files:**GS.hlsl")
        removeflags("ExcludeFromBuild")
        shadertype("Geometry")