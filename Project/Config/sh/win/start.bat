@echo off
pause

cd /d %~dp0
start "--name=master --type=0 --id=1" "%~dp0NetGrid.exe" --name=master --type=0 --id=1 --port=8001
start "--name=gate  " "%~dp0NetGrid.exe" --name=gate --type=1 --id=1 --port=8101
