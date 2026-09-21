
sudo apt update
sudo apt install -y python3
sudo apt install -y python3-pip

clear

if [ "$1" == "-f" ]; then

    rm -rf "philh_myftp_biz.egg-info"
    rm -rf "build"
    rm -rf "_build" 
    rm -rf "_api"
    rm -rf "dist"

    python3 -m pip uninstall -y philh_myftp_biz \
        --break-system-packages
    
    pip cache purge

fi

export PYTHONWARNINGS="ignore:setup.py install is deprecated"

pip_install() {
    python3 -m pip install "$@" \
        --break-system-packages \
        --root-user-action=ignore \
        --user
}

pip_install -vvv . --ignore-installed urllib3 || exit 1

# Update API Reference Docs
pip_install sphinx ghp-import
pip_install git+https://github.com/minefarts/sphinx-autodoc2
python3 -m sphinx -M html . _build -E -a
ghp-import -n -p -f _build/html

