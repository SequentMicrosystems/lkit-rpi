# Firmware install/update


### Download the lkit repository to your Raspberry Pi

Open a command prompt and type the following command:
```bash
~$ git clone https://github.com/SequentMicrosystems/lkit-rpi.git
```
or update it if is already cloned:

```bash
~$ cd lki-rpi
~/lkit-rpi$ git pull
```

### Update the card 
```bash
~$ cd lkit-rpi/update
~/lkit-rpi/update$ sudo ./update LKIT.hex
```
and follow the instructions
