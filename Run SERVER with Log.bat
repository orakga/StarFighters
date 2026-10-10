@echo off
rem Starts the dedicated server, with its log window, from this project's own build (Binaries\Win64).
setlocal
cd /d "%~dp0Binaries\Win64" 2>nul || (echo Not built yet: "%~dp0Binaries\Win64" doesn't exist. & pause & exit /b 1)
start "" SFServer.exe -log -servername="First Blank Server" -consolex=960 -consoley=540
exit /b 0
