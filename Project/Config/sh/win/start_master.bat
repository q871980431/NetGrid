@echo off
pause
set GridPath=%~dp0..\
cd /d %GridPath%
del /f /q .\log\*.log
start "--name=master --type=0 --id=1" "%GridPath%NetGrid.exe" --name=master --type=0 --id=1 --port=8001

