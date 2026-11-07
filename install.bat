@echo off
cls
pushd %~dp0

if "%~1"=="-f" (
    
    rmdir /s /q "philh_myftp_biz.egg-info"
    rmdir /s /q "build"
    rmdir /s /q "_build" 
    rmdir /s /q "_api"
    rmdir /s /q "dist"
    
    call :pip_remove philh_myftp_biz
    call :pip_remove auto-python-docs
    
    pip cache purge

)

set "PYTHONWARNINGS=ignore::UserWarning"

CALL :pip_install "-v" "." || exit /B

popd
goto :EOF

:pip_install
    python.exe -m pip install --user %*
    exit /B %ERRORLEVEL%

:pip_remove
    python.exe -m pip uninstall -y %*
    exit /B %ERRORLEVEL%

