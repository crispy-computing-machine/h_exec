@echo off
setlocal enableextensions enabledelayedexpansion

	cd /D %APPVEYOR_BUILD_FOLDER%
	if %errorlevel% neq 0 exit /b 3

    cd /d %APPVEYOR_BUILD_FOLDER%

    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
    cl %APPVEYOR_BUILD_FOLDER%\h_exec.c /MD /nologo  /Fe%APPVEYOR_BUILD_FOLDER%\h_exe.exe /link User32.lib

    echo Zipping Assets...
    7z a h_exec.zip %APPVEYOR_BUILD_FOLDER%\*
    appveyor PushArtifact h_exec-%APPVEYOR_REPO_TAG_NAME%.zip -FileName h_exec-%APPVEYOR_REPO_TAG_NAME%.zip

endlocal
