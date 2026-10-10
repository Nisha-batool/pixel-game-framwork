@echo off
cd /d "%~dp0"
if not exist "star_catcher.exe" (
	echo Star Catcher executable was not found in this folder.
	pause
	exit /b 1
)
start "" "%~dp0star_catcher.exe"
