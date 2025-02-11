@echo off
set BUILD_DIR=build

echo Building project...
cmake --build %BUILD_DIR%

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b %ERRORLEVEL%
)

echo Running executable...
%BUILD_DIR%\bin\main.exe
