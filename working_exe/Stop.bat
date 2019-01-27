@echo off

::Stop server
sc stop CS_IpCameraServerSvc
sc delete CS_IpCameraServerSvc
pause