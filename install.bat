@echo off
cls
cd /d "%~dp0"

if "%~1"=="-f" (
    
    rmdir /s /q "philh_myftp_biz.egg-info"
    rmdir /s /q "build"
    rmdir /s /q "dist"
    
    pip uninstall philh_myftp_biz -y
    
    pip cache purge

)

pip install . || exit /b 1

:: Locate package directory using pip
for /f "delims=" %%i in ('pip show philh_myftp_biz ^| findstr "Location:"') do set "DIR=%%i"
set "DIR=%DIR:~10%\philh_myftp_biz"

:: Precompile package
python -m compileall -f "%DIR%"
