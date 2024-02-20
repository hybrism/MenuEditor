mkdir bin\
mkdir bin\debug\
mkdir bin\release\
copy dependencies\dll\* bin\debug\
copy dependencies\dll\* bin\release\
copy dependencies\debug\dll\* bin\debug\
copy dependencies\release\dll\* bin\release\
copy dependencies\icon.ico bin\release\
copy dependencies\cursor.cur bin\debug\
copy dependencies\cursor.cur bin\release\
call "premake/premake5" --file=source/premake5.lua vs2022
pause
