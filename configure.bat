@echo off
set BUILD_DIR=build

echo Configuring project...
cmake -B %BUILD_DIR%

if %ERRORLEVEL% NEQ 0 (@echo off
set BUILD_DIR=build

echo Configuring project...
cmake -S . -B %BUILD_DIR%

if %ERRORLEVEL% NEQ 0 (
    echo Configuration failed!
    exit /b %ERRORLEVEL%
)

echo Configuration successful!

    echo Configuration failed!
    exit /b %ERRORLEVEL%
)
