@echo off
cls
cd /d "%~dp0"

set "f=0"
set "e=0"

:parse_args
if "%~1"=="" goto end_parse
if /I "%~1"=="-f" set "f=1"
if /I "%~1"=="-e" set "e=1"
shift
goto parse_args
:end_parse

if "%f%"=="1" (
    
    rmdir /s /q "philh_myftp_biz.egg-info"
    rmdir /s /q "build"
    rmdir /s /q "dist"
    del /q "philh_myftp_biz\*.pyd"
    
    pip uninstall philh_myftp_biz -y
    pip cache purge

)

if "%e%"=="1" (
    pip install -e .
) else (
    pip install .
)
IF %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

for /f "delims=" %%i in ('pip show philh_myftp_biz ^| findstr "Location:"') do set "DIR=%%i"
set "DIR=%DIR:~10%\philh_myftp_biz"

python -m compileall -f "%DIR%"
