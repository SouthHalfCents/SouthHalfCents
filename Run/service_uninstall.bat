rem @echo off
net stop AngoService

taskkill /f /im "Ango.exe"
taskkill /f /im "AngoTime.exe"

cd %~dp0
%~dp0AngoService.exe -u
pause