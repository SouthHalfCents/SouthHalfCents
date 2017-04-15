@echo off
sc create TestService binPath= "F:\Visual Studio 2013\Projects\testService\x64\Debug\testService.exe" start= auto 
sc config TestService type= interact type= own
net start TestService