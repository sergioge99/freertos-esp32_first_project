#!/bin/bash
. $HOME/esp/esp-idf/export.sh
sudo chmod 0666 /dev/ttyUSB0
cd pr2
idf.py set-target esp32
idf.py fullclean
idf.py build
idf.py -p /dev/ttyUSB0 flash
idf.py -p /dev/ttyUSB0 monitor
