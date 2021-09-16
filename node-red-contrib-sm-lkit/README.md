# node-red-contrib-sm-lkit

This is the node-red node to control Sequent Microsystems Learning Kit Card

## Manual Install

Clone or update the repository, follow the instrutions fron the [first page.](https://github.com/SequentMicrosystems/lkit-rpi)

In your node-red user directory, tipicaly ~/.node-red,

```bash
~$ cd ~/.node-red
```

Run the following command:

```bash
~/.node-red$ npm install ~/lkit-rpi/node-red-contrib-sm-lkit
```

In order to see the node in the palette and use-it you need to restart node-red. If you run node-red as a service:
 ```bash
 ~$ node-red-stop
 ~$ node-red-start
 ```

## Usage

After install and restart the node-red you will see on the node palete, under Sequent Microsystems category 6 new nodes:

### LKIT 0 10V in

This node reads the 0-10V input channel.
The read is triggered by the message input and output can be found in the output message payload as a number representing the voltage readings in volts.

### LKIT 0 10V out

This node controls the 0-10V output channel.
The value in volts is set dynamically as a number between 0..10 thru ```msg.payload```.

### LKIT 4 20mA in

This node reads the 4-20mA input channel. 
The read is triggered by the message input and output can be found in the output message payload as a number representing the current readings in milliamps.

### LKIT 4 20mA out

This node controls the 4-20mA output channel.
The value in milliamps is set dynamically as a number between 4..20 thru ```msg.payload```.

### LKIT MOT out

This node provide control to the card DC motor driver
You can control the torque and the direction of rotation setting ```msg.payload``` between -100 and 100.

### LKIT OPT in

This node reads the optically coupled input state for one particular channel.
The optically coupled input channel number can be set in the node dialog box or dynamically through ```msg.channel```.
The read is triggered by the message input and output can be found in the output message payload as boolean.

### LKIT RELAY

This node set one relay.
Set one relay with the value received in ```msg.payload```
You can specify the relay number in the edit dialog box or programaticaly with the input message ```msg.relay```.

### LKIT LED

This node provide access to the general purpose LED's on the card.
Set one led from 4 or all leds if ```led = 0``` with the value received in ```msg.payload```.
You can specify the led number in the edit dialog box or programaticaly with the input message ```msg.led```.


## Important note

This node is using the I2C-bus package from @fivdi, you can visit his work on github [here](https://github.com/fivdi/i2c-bus). 
The inspiration for this node came from @nielsnl68 work with [node-red-contrib-i2c](https://github.com/nielsnl68/node-red-contrib-i2c).
Thank both for the great job.
