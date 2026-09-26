
sudo apt update
sudo apt install -y python3
sudo apt install -y python3-pip

clear

pip_install() {
    python3 -m pip install "$@" \
        --break-system-packages \
        --root-user-action=ignore \
        --user
}

pip_remove() {
    python3 -m pip uninstall -y "$@" \
        --break-system-packages
}

if [ "$1" == "-f" ]; then

    rm -rf "philh_myftp_biz.egg-info"
    rm -rf "build"
    rm -rf "_build" 
    rm -rf "_api"
    rm -rf "dist"

    pip_remove philh_myftp_biz
    pip_remove auto-python-docs
    
    pip cache purge

fi

export PYTHONWARNINGS="ignore:setup.py install is deprecated"

pip_install -vvv . --ignore-installed urllib3 || exit 1

pip_install "auto-python-docs @ git+https://github.com/MineFartS/auto-python-docs"
python3 -m auto_python_docs.build

