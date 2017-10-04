Raspberry Pi Framebuffer Copy For Freeplay Zero and Freeplay CM3
================================================================

This program is used to copy primary framebuffer (eg. HDMI) to secondary framebuffer (eg. FBTFT).

Thanks to mrvanes for the callback-based fbcp functionality from https://github.com/mrvanes/rpi-fbcp


Prerequisites
-------------
    $ sudo apt-get install wiringpi libconfig-dev

Build
-----

    $ mkdir build
    
    $ cd build
    
    $ cmake ..
    
    $ make

    $ cd ..

Install
-------
    $ sudo cp build/Freeplay-fbcp /usr/local/bin

    $ sudo cp freeplayfbcp.cfg /boot

    $ sudo cp fbcp.sh /etc/init.d/

    $ sudo update-rc.d fbcp.sh defaults

Config
------
    $ sudo nano /boot/freeplayfbcp.cfg

    $ sudo service fbcp restart
