
@echo off
:::   (O_/ __ \_O)
:::   / (o)__(o) \
:::  (__.--\/--.__)
::: ====(__/\__)====
:::       `--'
:::       _||_
:::     /'....'\
:::    | :    : |
:::    ||:    :||
:::    ||:    :||
:::    ||:    :||
:::    ||:    :||
:::    ||:    :||
:::    ||:    :||
:::    || `..' ||
:::    ( | || | )
:::     \| || |/
:::      | || |

for /f "delims=: tokens=*" %%A in ('findstr /b ::: "%~f0"') do @echo(%%A

echo Are you sure you want to proceed? (Y/N)
set /p choice=

if /I "%choice%" NEQ "Y" (
    echo Operation cancelled.
    pause
    exit /b
)


:: CREATE ON THE GOOSE FOLDER
mkdir "%USERPROFILE%\Desktop\OnTheGoose"

:: CREATE A GAME FODLER INSIDE
mkdir "%USERPROFILE%\Desktop\OnTheGoose\Game"

:: COPY PASTE IMPORTANTS FOLDERS
xcopy /s /i "%~dp0Bin" "%USERPROFILE%\Desktop\OnTheGoose\Game\Bin"
xcopy /s /i "%~dp0Content" "%USERPROFILE%\Desktop\OnTheGoose\Game\Content"
xcopy /s /i "%~dp0Import" "%USERPROFILE%\Desktop\OnTheGoose\Game\Import"

cd..

:: COPY PASTE UNITY FOLDER
xcopy /s /i "P7_Grupp4_Unity" "%USERPROFILE%\Desktop\OnTheGoose\P7_Grupp4_Unity"

:: CLEAN UP 
cd "%USERPROFILE%\Desktop\OnTheGoose"

cd "Game"


::BIN

@cd "Bin"
@RD /S /Q "Editor"
@RD /S /Q "launcherDebug"


:: RELEASE
@cd "release"
@del /Q "*.pdb" 
@del /Q "*.lib" 
@del /Q "StartApplication_Release.exe"

cd..
cd..
cd..

:: UNITY 
@cd "P7_Grupp4_Unity"
@RD /S /Q ".\.vs"

@for /D %%i in (*) do (
    if /I "%%i" NEQ "Assets" (
        rd /s /q "%%i"
    )
)

@for %%f in (*) do (
    if /I "%%f" NEQ "Assets" (
        del /q "%%f"
    )
)

@cd "Assets"
@for /D %%i in (*) do (
    if /I "%%i" NEQ "Resources" (
        rd /s /q "%%i"
    )
)

@for %%f in (*) do (
    if /I "%%f" NEQ "Resources" (
        del /q "%%f"
    )
)

@cd "Resources"

for /D %%i in (*) do (
    if /I "%%i" NEQ "Skybox" (
        if /I "%%i" NEQ "Textures" (
            rem Remove the current directory and all its contents
            rd /s /q "%%i"
        )
    )
)

for %%f in (*) do (
    rem Remove all files in the "Resources" folder
    del /q "%%f"
)
pause