# lkit-rpi
System software for Sequent Microsystems Learning Kit card

## Step 1: Enable the I2C communication:
A good tutorial for this can be found [here](https://www.raspberrypi-spy.co.uk/2014/11/enabling-the-i2c-interface-on-the-raspberry-pi/).
## Step 2: Download the Learning Kit repository to your Raspberry Pi
```bash
~$ sudo apt-get install git
~$ git clone https://github.com/SequentMicrosystems/lkit-rpi.git
```

## Step3: Install the command line system
```bash
~$ cd lkit-rpi/
~/lkit-rpi$ sudo make install
```

If you downloaded the repository any update can be made with the following commands:

```bash
~$ cd lkit-rpi/  
~/lkit-rpi$ git pull
~/lkit-rpi$ sudo make install
```  

## Step4: Use the Learning kit with command line

Type ``` lkit -h ``` for the list of the available command options.



## [Firmware Update](https://github.com/SequentMicrosystems/lkit-rpi/blob/main/update/README.md)
