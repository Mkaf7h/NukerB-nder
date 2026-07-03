@echo off
echo ==========================================
echo    NUkerBinder Build Script
echo ==========================================

echo [1/5] Compiling Stub Engine...
gcc -O2 -s -mwindows -o NUkerStub.exe NUkerStub.c -lshell32 -lole32 -luser32 -ladvapi32
if %errorlevel% neq 0 goto error

echo [2/5] Compiling GenStub Utility...
gcc -O2 -o NUkerGenStub.exe NUkerGenStub.c
if %errorlevel% neq 0 goto error

echo [3/5] Generating Stub Data Header...
NUkerGenStub.exe NUkerStub.exe NUkerStubData.h
if %errorlevel% neq 0 goto error

echo [4/5] Compiling Icon Resource...
windres icon.rc -o icon.o
if %errorlevel% neq 0 goto error

echo [5/5] Compiling Main Application (New UI)...
gcc -O2 -s -mwindows -o NUkerBinder.exe NUkerBinder.c icon.o -lcomctl32 -lcomdlg32 -lshell32 -lole32 -lgdi32 -luser32 -ladvapi32
if %errorlevel% neq 0 goto error

echo.
echo ==========================================
echo    BUILD SUCCESSFUL!
echo    Output: NUkerBinder.exe
echo ==========================================
del NUkerGenStub.exe NUkerStub.exe icon.o
pause
exit /b 0

:error
echo.
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo    BUILD FAILED!
echo    Make sure MinGW (gcc/windres) is in PATH
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
pause
exit /b 1
