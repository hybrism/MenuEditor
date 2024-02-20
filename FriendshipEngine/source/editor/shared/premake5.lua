
project "Shared"
	location (dirs.shared)
	dependson { "external", "engine", "application" }
	
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	debugdir "%{dirs.bin}%{cfg.buildcfg}"
	targetdir ("%{dirs.bin}%{cfg.buildcfg}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")

	links { "external", "engine", "application" }

	includedirs
	{
		dirs.external,
		dirs.external .. "imgui/",
		dirs.engine,
		dirs.game
	}

	files {
		"**.h",
		"**.cpp",
		"**.hlsl",
        "**.hlsli",
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