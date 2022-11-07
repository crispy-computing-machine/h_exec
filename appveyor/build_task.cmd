@echo off
setlocal enableextensions enabledelayedexpansion

	cd /D %APPVEYOR_BUILD_FOLDER%
	if %errorlevel% neq 0 exit /b 3

    cd /d %APPVEYOR_BUILD_FOLDER%

    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" %ARCH%

    cl %APPVEYOR_BUILD_FOLDER%\h_exec.c /MD /nologo  /Fe%APPVEYOR_BUILD_FOLDER%\h_exe.exe User32.lib Advapi32.lib Shell32.lib

    echo Zipping Assets...
    7z a h_exec-%ARCH%.zip %APPVEYOR_BUILD_FOLDER%\h_exe.exe
    7z a h_exec-%ARCH%.zip %APPVEYOR_BUILD_FOLDER%\README.md
    7z a h_exec-%ARCH%.zip %APPVEYOR_BUILD_FOLDER%\README.md
    appveyor PushArtifact h_exec-%ARCH%.zip -FileName h_exec-%ARCH%.zip

endlocal
