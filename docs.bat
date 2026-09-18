@ECHO OFF

pushd %~dp0

echo Cleaning Workspace...
rmdir /s /q "_build" 
rmdir /s /q "_api"

echo Installing Dependencies...
pip install sphinx ghp-import
pip install git+https://github.com/minefarts/sphinx-autodoc2

echo Building HTML documentation...
python.exe -m sphinx -M html . _build -E -a

echo Deploying to GitHub Pages...
ghp-import -n -p -f _build\html

popd
