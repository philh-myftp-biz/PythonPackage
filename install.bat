@echo off
cls
pushd %~dp0

set "PYTHONWARNINGS=ignore::UserWarning"

CALL :pip_install "-v" "." || exit /B

popd
goto :EOF

:pip_install
    python.exe -m pip install --user %*
    exit /B %ERRORLEVEL%

