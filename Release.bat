C:\Factory\Tools\RDMD.exe /RC out

SET RAWKEY=eaf46a0c906dd165a5b37cb418b41aa7

C:\Factory\SubTools\makeCluster.exe Picture.txt     out\Picture.dat     %RAWKEY% 110000000
C:\Factory\SubTools\makeCluster.exe Music.txt       out\Music.dat       %RAWKEY% 120000000
C:\Factory\SubTools\makeCluster.exe SoundEffect.txt out\SoundEffect.dat %RAWKEY% 130000000
C:\Factory\SubTools\makeCluster.exe Etcetera.txt    out\Etcetera.dat    %RAWKEY% 140000000

COPY /B Hako2\Release\Hako2.exe out
COPY /B AUTHORS out
C:\Factory\Tools\xcp.exe doc out
ren out\Manual.txt ƒ}ƒjƒ…ƒAƒ‹.txt

out\Hako2.exe /L
IF ERRORLEVEL 1 START ?_LOG_ENABLED

MD out\Map
COPY /B res\_Dummy out\Map

MD out\Replay
COPY /B res\_Dummy out\Replay

C:\Factory\SubTools\zip.exe /G out Hako2
C:\Factory\Tools\summd5.exe /M out

IF NOT "%1" == "/-P" PAUSE
