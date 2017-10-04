#!/bin/sh
sudo apt-get install wiringpi libconfig-dev
mkdir build
cd build
cmake ..
make
cd ..
sudo cp build/Freeplay-fbcp /usr/local/bin
sudo cp freeplayfbcp.cfg /boot
sudo cp fbcp.sh /etc/init.d/
sudo update-rc.d /etc/init.d/fbcp.sh defaults
sudo nano /boot/freeplayfbcp.cfg
sudo service fbcp restart
