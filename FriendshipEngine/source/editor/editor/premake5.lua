project "Editor"
	location (dirs.editor)
	dependson { "external", "engine", "application", "game", "ecs", "shared" }
	
	language "C++"
	cppdialect "C++17"

	debugdir "%{dirs.bin}%{cfg.buildcfg}"
	targetdir ("%{dirs.bin}%{cfg.buildcfg}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")

	links { "external", "engine", "application", "game", "ecs", "shared" }

	includedirs
	{
		dirs.external,
		dirs.external .. "imgui/",
		dirs.engine,
		dirs.editor,
		dirs.game
	}

	files {
		"**.h",
		"**.cpp",
		"**.hlsl",
        "**.hlsli",
	}

	links {
		"DirectXTex.lib",
		"DirectXTex_Spectre.lib",
	}
	--verify_or_create_settings("Game")

	filter "configurations:Editor"
		defines "_EDITOR"
		runtime "Debug"
		symbols "on"
		libdirs { dirs.dependencies_debug }	
		kind "StaticLib"
	filter "configurations:LauncherDebug"
		defines "_LAUNCHER"
		runtime "Debug"
		symbols "on"
		libdirs { dirs.dependencies_debug }
		kind "None"
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
		libdirs { dirs.dependencies_release }	
		kind "None"
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