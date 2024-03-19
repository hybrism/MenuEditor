mkdir bin\
mkdir bin\editor\
mkdir bin\launcherDebug\
mkdir bin\release\

copy dependencies\dll\* bin\editor\
copy dependencies\debug\dll\* bin\editor\
copy dependencies\cursor.cur bin\editor\
copy dependencies\icon.ico bin\editor\

copy dependencies\dll\* bin\launcherDebug\
copy dependencies\debug\dll\* bin\launcherDebug\
copy dependencies\cursor.cur bin\launcherDebug\
copy dependencies\icon.ico bin\launcherDebug\

copy dependencies\dll\* bin\release\
copy dependencies\release\dll\* bin\release\
copy dependencies\icon.ico bin\release\
copy dependencies\cursor.cur bin\release\


copy dependencies\cursor.cur bin\release\
call "premake/premake5" --file=source/premake5.lua vs2022
pause
