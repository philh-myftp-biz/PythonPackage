@echo off
cls
pushd %~dp0

set "PYTHONWARNINGS=ignore::UserWarning"

CALL :pip_install "-v" "." || exit /B

:: Locate package directory using pip
for /f "delims=" %%i in ('pip show philh_myftp_biz ^| findstr "Location:"') do set "DIR=%%i"
set "DIR=%DIR:~10%\philh_myftp_biz"

:: Precompile package
python -m compileall -f "%DIR%"

popd
goto :EOF

:pip_install
    python.exe -m pip install --user %*
    exit /B %ERRORLEVEL%

