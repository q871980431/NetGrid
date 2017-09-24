@echo off
pause

cd /d %~dp0
start "agent --name=agent --agent=11401" "%~dp0NetGrid.exe" --name=gate --type=1 --id=1 --port=11101