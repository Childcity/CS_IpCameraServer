@echo off

::Start server as service
sc create CS_IpCameraServerSvc binpath=%~dp0CS_IpCameraServer.exe
sc description CS_IpCameraServerSvc "Service for CS_IpCameraServer"
sc start CS_IpCameraServerSvc
pause