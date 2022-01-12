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

When used in _input_ mode the node shows only one port, the output port.  The node monitors the input designated in the _input channel_ dropdown menu. Whenever the selected _input channel_ changes a new output will be produced indicating the new state of the channel (grouneded or open).

In _input_ mode **no** output is produced upon intial deployment of the node.

#### Read Mode Operation
In read mode both an input and output port are shown. An output message is generated whenever a message is received at the input port.  The contents of the <span style = "color:red">*msg.payload*</span> are ignored and the output is triggerd only by the reception of the message.

The _input channel_ to be read may be designated either in the input channel dropdown menu or by the <span style = "color:red">*msg.channel*</span> value of the input message.  To use the <span style = "color:red">*msg.channel*</span> value delete the channel number from the _input channel_ dropdown.

The only valid value for <span style = "color:red">*msg.channel*</span> are numeric 1, 2, 3, 4 and strings "1","2", "3", and "4".  All other values will produce an error message and no output will be produced.

**Debouncing**: the value of the _input channel_ is determined by reading the signal at a short intervals (default of 25 msec) until two reads produce the same value. This becomes the output value. In _input_ mode an output is only produced if two successive reads differ from the current store value.

### LKIT RELAY

This node set one relay.
Set one relay with the value received in ```msg.payload```
You can specify the relay number in the edit dialog box or programaticaly with the input message ```msg.relay```.

### LKIT LED

Controls LED GP1 - GP4 on the Sequent Microsystems Learning Kit Card.
There are two modes of operation: *single mode* and *group* mode.  The mode is selected in the node edit

#### Single Mode Operation
In single mode operation <span style = "color:red;">*msg.payload*</span> controls the state of a single LED.  The LED to be controlled may be defined in the node editor or in <span style = "color:red">*msg.led*</span>.

The <span style = "color:red;">*msg.payload*</span> value may be numeric, string or Boolean.

 * 0, "0","off", false - Set LED off
 * 1, "1","on", true   - set LED on

All other values will result in an error message.

##### Selecting and LED in Single Mode

The LED to control is selected either in the node editor or by *msg.led*.  If *msg.led* is define it has priority over the selection in the node editor.

The <span style = "color:red">*msg.led*</span> value may be numeric or string.

 * 1, "1" - Select LED GP1
 * 2, "2" - Select LED GP2
 * 3, "3" - Select LED GP3
 * 4, "4" - Select LED GP4
 

#### Group Mode Operation

In group mode <span style = "color:red;">*msg.payload*</span> control all four LEDs (GP1 - GP4) as unit.

The <span style = "color:red;">*msg.payload*</span> value may be numeric or string.

*numeric - 0, 1, 2... 15
*string - "0", "1", "2"... "15"

The value of the <span style = "color:red;">*msg.payload*</span> value is interpred converted in to a binary number and applied to all four LEDs.  GP1 is the least significan bit and GP4 is the most significan bit. For example, and AN *msg.led* balue of 0 or "0" means all LEDs are off and an *msg.led" of 15 or "15" means all LEDs are on.

All other values of *<span style = "color:red;">*msg.payload*</span> in group mode will result in an error message.

### LKIT BUTTON

This node provide acces to the pushbutton on the card.
It sed the sate of the button only if is changed thru ```msg.payload```.

## Important note

This node is using the I2C-bus package from @fivdi, you can visit his work on github [here](https://github.com/fivdi/i2c-bus). 
The inspiration for this node came from @nielsnl68 work with [node-red-contrib-i2c](https://github.com/nielsnl68/node-red-contrib-i2c).
Thank both for the great job.
