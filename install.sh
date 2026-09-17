
sudo apt update
sudo apt install -y python3
sudo apt install -y python3-pip

clear

if [ "$1" == "-f" ]; then

    rm -rf "philh_myftp_biz.egg-info"
    rm -rf "build"
    rm -rf "dist"

    python3 -m pip uninstall -y philh_myftp_biz \
        --break-system-packages
    
    pip cache purge

fi

export PYTHONWARNINGS="ignore:setup.py install is deprecated"
export PIP_ROOT_USER_ACTION="ignore"

python3 -m pip install -vvv . \
    --break-system-packages \
    --root-user-action=ignore \
    --ignore-installed urllib3

