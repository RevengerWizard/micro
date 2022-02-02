:: a small batch file to export all the assets for the game

@echo off

:: use %~dp0 to get the batch script's path, pretty useful
cd %~dp0

set assetsPath=%~dp0assets
set animationsPath=%~dp0data

set charactersPath=%assetsPath%\characters
set itemsPath=%assetsPath%\items
set tilesetsPath=%assetsPath%\tilesets

set asepritesCharactersPath=%~dp0aseprite\characters\
set asepritesItemsPath=%~dp0aseprite\items\
set asepritesTilesetsPath=%~dp0aseprite\tilesets\

:: path to Aseprite
@set ASEPRITE="C:\Program Files\Aseprite\aseprite.exe"

:: use %%~na to get variable %%a without extension, nice

:: get all characters aseprite files and export spritesheet + json file (array and only tags)
for /f %%a in ('dir /b /s %asepritesCharactersPath%*.aseprite') do call %ASEPRITE% -b %%a --sheet-pack --sheet %charactersPath%\%%~na.png --data %animationsPath%\%%~na.json --format json-array --list-tags

:: get all items aseprite files and export spritesheet
::aseprite.exe -b image.ase --save-as image.png
::for /f %%b in ('dir /b /s %asepritesItemsPath%*.aseprite') do call %ASEPRITE% -b %%b --save-as %itemsPath%\%%~nb.png

:: get all tilesets aseprite files and export spritesheet
::for /f %%b in ('dir /b /s %asepritesTilesetsPath%*.aseprite') do call %ASEPRITE% -b %%b --save-as %tilesetsPath%\%%~nb.png

:: to call and regenerate the JSON files
python auto.py