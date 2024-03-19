----------------------------------------------------------------------------
-- the dirs table is a listing of absolute paths, since we generate projects
-- and files it makes a lot of sense to make them absolute to avoid problems
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
dirs = {}
dirs["root"] 			= os.realpath("../")
dirs["bin"]				= os.realpath(dirs.root .. "bin/")
dirs["temp"]			= os.realpath(dirs.root .. "intermediate/")
--dirs["lib"]				= os.realpath(dirs.root .. "lib/")
dirs["projectfiles"]	= os.realpath(dirs.root .. "local/")
dirs["source"] 			= os.realpath(dirs.root .. "source/")
dirs["dependencies_debug"]	= os.realpath(dirs.root .. "dependencies/debug")
dirs["dependencies_release"]	= os.realpath(dirs.root .. "dependencies/release")
dirs["settings"]		= os.realpath(dirs.bin .. "settings/")
dirs["content"] 	= os.realpath(dirs.root .. "content/")

dirs["game"] 			= os.realpath(dirs.source .. "game/")
dirs["game_core"] 			= os.realpath(dirs.game .. "game/")

dirs["external"]		= os.realpath(dirs.source .. "external/")
dirs["engine"]			= os.realpath(dirs.source .. "engine/")
dirs["engine_core"]			= os.realpath(dirs.engine .. "engine/")
dirs["engine_ecs"]			= os.realpath(dirs.engine .. "ecs/")
dirs["engine_assets"]			= os.realpath(dirs.engine .. "assets/")
dirs["engine_application"]			= os.realpath(dirs.engine .. "application/")
dirs["launcher"]			= os.realpath(dirs.source .. "launcher/")
dirs["start_application"]			= os.realpath(dirs.source .. "startApplication/")
dirs["editor"]			= os.realpath(dirs.source .. "editor/")
dirs["editor_core"]		= os.realpath(dirs.editor .. "editor/")
dirs["modelViewer"]		= os.realpath(dirs.editor .. "modelViewer/")
dirs["menuEditor"]		= os.realpath(dirs.editor .. "menuEditor/")
dirs["shared"]		= os.realpath(dirs.editor .. "shared/")

dirs["shaders"]	=  os.realpath(dirs.bin .. "shaders/")

--dirs.shaders["relative"] = "../bin/shaders/"

engine_settings = os.realpath(dirs.settings .. "/EngineSettings.json")


-----------------------------------------------------------------------
-- These should be more or less equivalent with Engines WindowConfiguration struct
-- Some of it can't be set like this, things like callbacks.
--function default_settings()
--	return {
--		assets_path = {
--			engine = {
--				absolute = path.translate(dirs.content, "/"),
--				relative = path.getrelative(dirs.bin, dirs.content) .. "/"
--			},
--			game = {
--				absolute = path.translate(dirs.content, "/"),
--				relative = path.getrelative(dirs.bin, dirs.content) .. "/"
--			}
--		},
--
--		window_settings = {
--			window_size = {w=1600, h=900},
--			render_size = {w=1600, h=900},
--			target_size = {w=1600, h=900},
--			title = "FriendshipEngine",
--	 		clear_color = {r=0.0, g=0.2, b=0.25, a=1.0},
--
--			use_letterbox_and_pillarbox = false,
--			keep_aspect_ratio = true,
--			aspect_ratio = 1.7,
--
--	 		start_in_fullscreen = false
--		},
--		enable_vsync = true,
--	}
--end

if not os.isdir (dirs.bin) then
	os.mkdir (dirs.bin)
end

--if not os.isdir(dirs.settings) then 
--	os.mkdir (dirs.settings)
--end

---------------------------------------------------------------------------
-- Utility function to create individual project-settings any game project, 
-- tutorial or similar is responsible for running:
-- Tga::LoadSettings("game_name.json"); at startup
--function verify_or_create_settings(game_name)
--	local settings_filename = game_name .. ".json"
--	defines { 'TGE_PROJECT_SETTINGS_FILE="' .. settings_filename .. '"' }
--	local game_settings = dirs["settings"] .. settings_filename
--	
--	local settings = default_settings()
--	if os.isfile(game_settings) then
--		local old_settings = json.decode(io.readfile(game_settings))
--		for k,v in pairs(old_settings) do
--			settings[k] = v
--		end
--
--		--settings.assets_path.engine.absolute = path.translate(dirs.content, "/")
--		settings.assets_path.game.absolute = path.translate(dirs.content, "/")
--	end
--
--	io.writefile(
--		game_settings,
--		json.encode(settings)
--	)
--end