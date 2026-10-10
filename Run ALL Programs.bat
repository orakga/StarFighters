@echo off
rem Starts the dedicated server and three game clients from this project's own build
rem (Binaries\Win64): build the "Development Server" and "Development Client" targets first.
rem Works wherever the project is cloned. Window positions fit one 1920 x 1080 screen:
rem clients top-left, top-right and bottom-left, the logs stacked bottom-right.
setlocal

rem ---- Settings: change these to suit your machine ----
rem The server's address: 127.0.0.1 is this PC. To join a server on another PC, use its IP.
set "SERVER_IP=127.0.0.1"
rem Each client window's size. Three of them fit one 1920 x 1080 screen with the server's log.
set "RES_X=960"
set "RES_Y=540"
rem ------------------------------------------------------

cd /d "%~dp0Binaries\Win64" 2>nul || (echo Not built yet: "%~dp0Binaries\Win64" doesn't exist. & pause & exit /b 1)

start "" SFServer.exe -log -servername="First Blank Server" -consolex=960 -consoley=540

start "" SFClient.exe -windowed resx=%RES_X% resy=%RES_Y% -log -playername="P1" -serverip="%SERVER_IP%" -winx=0 -winy=0 -consolex=990 -consoley=570
start "" SFClient.exe -windowed resx=%RES_X% resy=%RES_Y% -log -playername="P2" -serverip="%SERVER_IP%" -winx=960 -winy=0 -consolex=1020 -consoley=600
start "" SFClient.exe -windowed resx=%RES_X% resy=%RES_Y% -log -playername="P3" -serverip="%SERVER_IP%" -winx=0 -winy=540 -consolex=1050 -consoley=630

exit /b 0
