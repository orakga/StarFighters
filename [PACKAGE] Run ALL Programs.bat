
cd "F:\UE Packages\Star Fighters\WindowsServer\StarFighters\Binaries\Win64"

start SFServer.exe -log -servername="First Blank Server (Pkg)" -consolex=0 -consoley=0

cd "F:\UE Packages\Star Fighters\WindowsClient\StarFighters\Binaries\Win64"

start SFClient.exe -windowed resx=1280 resy=720 -log -playername="P1 (Pkg)" -serverip="192.168.1.10" -consolex=0 -consoley=550 -winx=0 -winy=1100
start SFClient.exe -windowed resx=1280 resy=720 -log -playername="P2 (Pkg)" -serverip="192.168.1.10" -consolex=1280 -consoley=550 -winx=1280 -winy=1100
start SFClient.exe -windowed resx=1280 resy=720 -log -playername="P3 (Pkg)" -serverip="192.168.1.10" -consolex=2560 -consoley=550 -winx=2560 -winy=1100

exit
