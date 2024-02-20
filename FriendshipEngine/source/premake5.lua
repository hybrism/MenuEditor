include "../premake/extensions.lua"
-- include for common stuff 
include "../premake/common.lua"

workspace "FriendshipEngine"
	location "../"
	startproject "Editor"
	architecture "x64"

	configurations {
		"Debug",
		"Release"
	}

include (dirs.external)
include (dirs.engine)
include (dirs.game)
include (dirs.launcher)
include (dirs.editor)
