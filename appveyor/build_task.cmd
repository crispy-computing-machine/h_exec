@echo off
setlocal enableextensions enabledelayedexpansion

	cd /D %APPVEYOR_BUILD_FOLDER%
	if %errorlevel% neq 0 exit /b 3

    cd /d %APPVEYOR_BUILD_FOLDER%

    cl %APPVEYOR_BUILD_FOLDER%\h_exec.c /MD /nologo  /Fe%APPVEYOR_BUILD_FOLDER%\h_exe.exe

    echo Zipping Assets...
    7z a h_exec.zip %APPVEYOR_BUILD_FOLDER%\*
    appveyor PushArtifact h_exec-%APPVEYOR_REPO_TAG_NAME%.zip -FileName h_exec-%APPVEYOR_REPO_TAG_NAME%.zip

endlocal
