@echo off
setlocal enableextensions enabledelayedexpansion
	cinst wget

	if not exist "%PHP_BUILD_CACHE_BASE_DIR%" (
		echo Creating %PHP_BUILD_CACHE_BASE_DIR%
		mkdir "%PHP_BUILD_CACHE_BASE_DIR%"
	)

	if not exist "%PHP_BUILD_OBJ_DIR%" (
		echo Creating %PHP_BUILD_OBJ_DIR%
		mkdir "%PHP_BUILD_OBJ_DIR%"
	)

	if "%APPVEYOR%" equ "True" rmdir /s /q C:\cygwin >NUL 2>NUL
	if %errorlevel% neq 0 exit /b 3
	if "%APPVEYOR%" equ "True" rmdir /s /q C:\cygwin64 >NUL 2>NUL
	if %errorlevel% neq 0 exit /b 3
	if "%APPVEYOR%" equ "True" rmdir /s /q C:\mingw >NUL 2>NUL
	if %errorlevel% neq 0 exit /b 3
	if "%APPVEYOR%" equ "True" rmdir /s /q C:\mingw-w64 >NUL 2>NUL
	if %errorlevel% neq 0 exit /b 3

	if "%APPVEYOR_REPO_TAG_NAME%"=="" (
		set APPVEYOR_REPO_TAG_NAME=%APPVEYOR_REPO_BRANCH%-%APPVEYOR_REPO_COMMIT:~0,8%
		appveyor SetVariable -Name APPVEYOR_REPO_TAG_NAME -Value !APPVEYOR_REPO_TAG_NAME!
	)
endlocal