@echo off
rem Starts one game client in a window, from this project's own build (Binaries\Win64).
rem Double-click for player 1, or give the player number: "Run CLIENT in Window.bat" 2
setlocal

rem ---- Settings: change these to suit your machine ----
rem The server's address: 127.0.0.1 is this PC. To join a server on another PC, use its IP.
set "SERVER_IP=127.0.0.1"
rem Each client window's size. Three of them fit one 1920 x 1080 screen with the server's log.
set "RES_X=960"
set "RES_Y=540"
rem ------------------------------------------------------

set "N=%~1"
if "%N%"=="" set "N=1"
set "X=0" & set "Y=0"
if "%N%"=="2" set "X=960"
if "%N%"=="3" set "Y=540"

cd /d "%~dp0Binaries\Win64" 2>nul || (echo Not built yet: "%~dp0Binaries\Win64" doesn't exist. & pause & exit /b 1)
start "" SFClient.exe -windowed resx=%RES_X% resy=%RES_Y% -log -playername="P%N%" -serverip="%SERVER_IP%" -winx=%X% -winy=%Y%
exit /b 0
