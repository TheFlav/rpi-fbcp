#!/bin/sh
sudo apt-get install wiringpi libconfig-dev
mkdir build
cd build
cmake ..
make
cd ..
sudo service fbcp stop
sudo cp build/Freeplay-fbcp /usr/local/bin
sudo cp freeplayfbcp.cfg /boot
sudo cp fbcp.sh /etc/init.d/
sudo update-rc.d fbcp.sh defaults
sudo service fbcp restart
sudo systemctl daemon-reload
