@echo off
rem Generates TerrariaClone.sln for Visual Studio 2022 (downloads Premake if needed).
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build.ps1" -GenerateOnly
if errorlevel 1 pause & exit /b 1
