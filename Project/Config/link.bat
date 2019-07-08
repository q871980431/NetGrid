@echo off
pause

cd /d %~dp0
mklink /d "%~dp0\core" D:\Git\NetGrid\Project\Config\core
mklink /d "%~dp0\envir" D:\Git\NetGrid\Project\Config\envir

pause
