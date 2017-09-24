@echo off
pause

cd /d %~dp0
start "agent --name=master --agent=11401" "%~dp0NetGrid.exe" --name=master --type=0 --id=1 --port=7001