project "PhysX"
	location "%{dirs.localdir}"
		
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetdir ("%{dirs.lib}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")

	files {
		"./**.h",
		"./**.hpp",
		"./**.cpp",
		"./**.c",
		"./**.natvis",
		"./**.natstepfilter",
	}
	includedirs {
		".",
	}
    libdirs { dirs.lib }
	filter "configurations:Debug"
		defines "_LAUNCHER"
		runtime "Debug"
		symbols "on"
	