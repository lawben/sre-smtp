@echo off

set CMAKE=cmake
set CMAKE_GENERATOR="Visual Studio 15 Win64"

set ROOT_DIR=%CD%\..\..
set BUILD_DIR=%ROOT_DIR%\build

if not exist configure.bat (
	echo No custom configuration found.
    echo     Add buildscripts\win\configure.bat for custom configuration.
) else (
	echo Use custom configuration.
)