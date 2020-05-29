@echo off
pause

set GridPath=%~dp0..\
cd /d %GridPath%
del /f /q .\log\*.log
pause
start "agent --name=relation" "%GridPath%NetGrid.exe" --name=relation --type=3 --id=1 --port=8301
