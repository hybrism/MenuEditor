@echo off

echo Building the solution...

set "MSBuildPath=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

set "ProjectPath=%~dp0source\launcher\StartApplication.vcxproj"

"%MSBuildPath%" "%ProjectPath%" /p:Configuration=Release /p:Platform=x64 /t:Build

echo. & echo. Build completed.


pause
