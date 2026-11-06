
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

export PYTHONWARNINGS="ignore:setup.py install is deprecated"

pip_install -v . --ignore-installed urllib3 || exit 1

