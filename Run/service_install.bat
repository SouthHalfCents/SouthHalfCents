rem @echo off
cd %~dp0
%~dp0AngoService.exe -i
net start AngoService
pause