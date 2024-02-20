#pragma once

#ifdef _DEBUG
// --- Engine settings ---
#define EDITOR_VIEWPORT_SIZE 1
#define SHOW_CONSOLE_WINDOW 1


// --- Editor settings ---
#define START_IN_PLAYER_MODE 0
#define USE_SCRIPT_EDITOR 1
#define START_WITH_MAINMENU 0

// --- "Retail" ---
#define USE_LAUNCHER_AS_STARTUP 0


#else
// --- Build mode --- 
// (shouldn't need this if we use #ifdef _DEBUG but added just in case)
#define EDITOR_VIEWPORT_SIZE 0
#define SHOW_CONSOLE_WINDOW 0
#define USE_LAUNCHER_AS_STARTUP 1

#endif
