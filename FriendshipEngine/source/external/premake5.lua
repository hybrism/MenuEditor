group ("External")

--include "assimp"
--include "imgui"
--include "Inc"
--include "stb"

--group ("")

project "External"
	location (dirs.external)
		
	language "C++"
	cppdialect "C++17"
	kind "StaticLib"

	debugdir "%{dirs.bin}%{cfg.buildcfg}"
	targetdir ("%{dirs.bin}%{cfg.buildcfg}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")

	files {
		"**.h",
		"**.hpp",
		"**.inl",
		"**.c",
		"**.cpp",
	}

	excludes {
		--"ffmpeg-2.0/**.h",
		--"ffmpeg-2.0/**.c",
		--"ffmpeg-2.0/**.cpp",
	}

	includedirs {
		".",
		"source/",
		"imgui/",
		"fbxsdk/",
		"PhysX/",
		"directxtk/"
	}

	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
		--files {"tools/**"}
		--includedirs {"tools/"}
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
		--files {"tools/**"}
		--includedirs {"tools/"}
	--libdirs { "Lib/" }
