#!/bin/bash

. $HOME/esp/esp-idf/export.sh # Allow running of the .py files
idf.py set-target esp32
idf.py build
