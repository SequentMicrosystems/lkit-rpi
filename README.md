# lkit-rpi
Command Line Interface for Sequent Microsystems Learning Kit card

You need to enable the I2C communication:
```bash
~$ sudo raspi-config
```
## Install the command line system

```bash
~$ sudo apt-get install git
~$ git clone https://github.com/SequentMicrosystems/lkit-rpi.git
~$ cd lkit-rpi/
~/lkit-rpi$ sudo make install
```

Now you can access all the functions of the relays board through the command "lkit". Use -h option for help:
```bash
~$ lkit -h
```

If you clone the repository any update can be made with the following commands:

```bash
~$ cd lkit-rpi/  
~/lkit-rpi$ git pull
~/lkit-rpi$ sudo make install
```  

## [Firmware Update](https://github.com/SequentMicrosystems/lkit-rpi/blob/main/update/README.md)
