@echo off
rem Starts a packaged server and three packaged clients (Platforms > Windows > Package Project,
rem once for the Server target and once for the Client target, into the same folder).
rem Set PACKAGES to that folder. Window positions fit one 1920 x 1080 screen.
setlocal

rem The folder you packaged into; it holds WindowsServer\ and WindowsClient\.
set "PACKAGES=%~dp0Packaged"
rem ---- Settings: change these to suit your machine ----
rem The server's address: 127.0.0.1 is this PC. To join a server on another PC, use its IP.
set "SERVER_IP=127.0.0.1"
rem Each client window's size. Three of them fit one 1920 x 1080 screen with the server's log.
set "RES_X=960"
set "RES_Y=540"
rem ------------------------------------------------------

cd /d "%PACKAGES%\WindowsServer\StarFighters\Binaries\Win64" 2>nul || (echo No packaged server in "%PACKAGES%\WindowsServer": set PACKAGES in this file. & pause & exit /b 1)
start "" SFServer.exe -log -servername="First Blank Server (Pkg)" -consolex=960 -consoley=540

cd /d "%PACKAGES%\WindowsClient\StarFighters\Binaries\Win64" 2>nul || (echo No packaged client in "%PACKAGES%\WindowsClient": set PACKAGES in this file. & pause & exit /b 1)
start "" SFClient.exe -windowed resx=%RES_X% resy=%RES_Y% -log -playername="P1 (Pkg)" -serverip="%SERVER_IP%" -winx=0 -winy=0 -consolex=990 -consoley=570
start "" SFClient.exe -windowed resx=%RES_X% resy=%RES_Y% -log -playername="P2 (Pkg)" -serverip="%SERVER_IP%" -winx=960 -winy=0 -consolex=1020 -consoley=600
start "" SFClient.exe -windowed resx=%RES_X% resy=%RES_Y% -log -playername="P3 (Pkg)" -serverip="%SERVER_IP%" -winx=0 -winy=540 -consolex=1050 -consoley=630

exit /b 0
