
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

python3 -m pip install -vvv . \
    --break-system-packages \
    --ignore-installed urllib3

