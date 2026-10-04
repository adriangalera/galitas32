@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0run_verification.ps1" %*
exit /b %ERRORLEVEL%
