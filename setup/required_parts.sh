#!/bin/bash

# From https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html

sudo apt-get install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0

mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git

cd ~/esp/esp-idf
./install.sh esp32

# When running from terminal --> Run . $HOME/esp/esp-idf/export.sh      to allow compiling 