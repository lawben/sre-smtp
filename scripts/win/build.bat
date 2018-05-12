@echo off

goto :MAIN

:buildProject
    echo build sre-smtp
	cd %ROOT_DIR%
    if not exist %BUILD_DIR% (
        mkdir %BUILD_DIR%
    )
    cd %BUILD_DIR%

    %CMAKE% -DCMAKE_BUILD_TYPE=Debug -G %CMAKE_GENERATOR% ..
    %CMAKE% --build "%~1" --config %3 --target ALL_BUILD

    cd %ROOT_DIR%
    exit /B 0

:build
    call :buildProject
    exit /B 0

:clean
    if exist %BUILD_DIR% (
        echo clean %BUILD_DIR%
	    rmdir /S /Q %BUILD_DIR%
    )
	exit /B 0

:rebuild
	call :clean
	call :build
	exit /B 0

:help
	echo Usage: "build <target>"
    echo.
    echo build          Builds the sre-smtp project.
    echo clean          Deletes build folder.
    echo help           Print these informations.
    echo rebuild        Cleans the project and builds it from scratch.
    echo.
    echo.
    exit /B 0

:MAIN

set CALLING_DIR=%CD%

if "%~1"=="" (
    echo No build target specified.
    call :help
) else (
	call defaultconfig.bat
	call :%1%
)

cd %CALLING_DIR%
Pause