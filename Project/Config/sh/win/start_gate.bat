@echo off
pause

set GridPath=%~dp0..\
cd /d %GridPath%
del /f /q .\log\*.log
pause
start "agent --name=gate" "%GridPath%NetGrid.exe" --name=gate --type=1 --id=1 --port=8101
