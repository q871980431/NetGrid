@echo off
cd /d %~dp0
FOR /R ./ %%F IN (*.proto) DO protoc --proto_path=%~dp0 --cpp_out=. "%%F"
FOR /R ./ %%F IN (*.proto) DO protoc --proto_path=%~dp0 --csharp_out=. "%%F"

pause.