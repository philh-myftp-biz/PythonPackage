@echo off
cls
pushd %~dp0

if "%~1"=="-f" (
    
    rmdir /s /q "philh_myftp_biz.egg-info"
    rmdir /s /q "build"
    rmdir /s /q "_build" 
    rmdir /s /q "_api"
    rmdir /s /q "dist"
    
    pip uninstall philh_myftp_biz -y
    
    pip cache purge

)

set "PYTHONWARNINGS=ignore::UserWarning"

CALL :pip_install "-vvv" "."

:: Locate package directory using pip
for /f "delims=" %%i in ('pip show philh_myftp_biz ^| findstr "Location:"') do set "DIR=%%i"
set "DIR=%DIR:~10%\philh_myftp_biz"

:: Precompile package
python -m compileall -f "%DIR%"

:: Update API Reference Docs
call :pip_install "sphinx" "ghp-import"
call :pip_install "git+https://github.com/minefarts/sphinx-autodoc2"
python.exe -m sphinx -M html . _build -E -a
ghp-import -n -p -f _build\html

popd
goto :EOF

:pip_install
    python.exe -m pip install --user %* || goto :EOF
    exit /B

