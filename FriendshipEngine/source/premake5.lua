include "../premake/extensions.lua"
-- include for common stuff 
include "../premake/common.lua"

workspace "FriendshipEngine"
	location "../"
	startproject "StartApplication"
	architecture "x64"

	configurations {
		"Editor",
		"LauncherDebug",
		"Release"
	}

include (dirs.external)
include (dirs.engine)
include (dirs.game)
include (dirs.launcher)
include (dirs.editor)
include (dirs.start_application)
