module.exports = function (RED) {
  "use strict";
  var I2C = require("i2c-bus");
  var rpio = require("rpio");

  const DEFAULT_HW_ADD = 0x05;
  const I2C_MEM_U0_10_OUT_VAL1 = 18;
  const I2C_MEM_I4_20_OUT_VAL1 = 16;
  const I2C_MEM_MOT_PWM1 = 20;
  const I2C_MEM_U0_10_IN_VAL1 = 10;
  const I2C_MEM_I4_20_IN_VAL1 = 12;
  const I2C_MEM_LED_SET_ADD = 22;
  const I2C_MEM_LED_CLR_ADD = 23;
  const I2C_MEM_LED_VAL_ADD = 24;

  const gInPins = new ArrayBuffer(4);
  gInPins[0] = 7; //4;
  gInPins[1] = 11; //17;
  gInPins[2] = 13; //27;
  gInPins[3] = 15; //22;

  const gRelayPins = new ArrayBuffer(2);
  gRelayPins[0] = 35; //19;
  gRelayPins[1] = 36; //16;

  function VInNode(n) {
    RED.nodes.createNode(this, n);
    this.payload = n.payload;
    this.payloadType = n.payloadType;
    var node = this;
    var buffer = Buffer.alloc(2);

    node.port = I2C.openSync(1);
    node.on("input", function (msg) {
      var myPayload;

      try {
        var hwAdd = DEFAULT_HW_ADD;

        if (this.payloadType == null) {
          myPayload = this.payload;
        } else if (this.payloadType == "none") {
          myPayload = null;
        } else {
          myPayload = RED.util.evaluateNodeProperty(
            this.payload,
            this.payloadType,
            this,
            msg
          );
        }
        node.port.readI2cBlock(
          hwAdd,
          I2C_MEM_U0_10_IN_VAL1,
          2,
          buffer,
          function (err, size, res) {
            if (err) {
              node.error(err, msg);
            } else {
              msg.payload = res.readIntLE(0, 2) / 1000.0;
              node.send(msg);
            }
          }
        );
      } catch (err) {
        this.error(err, msg);
      }
    });

    node.on("close", function () {
      node.port.closeSync();
    });
  }
  RED.nodes.registerType("LKIT 0-10V in", VInNode);

  function VOutNode(n) {
    RED.nodes.createNode(this, n);
    this.payload = n.payload;
    this.payloadType = n.payloadType;
    var node = this;
    var buffer = Buffer.alloc(2);

    node.port = I2C.openSync(1);
    node.on("input", function (msg) {
      var myPayload;
      //var buffcount = parseInt(node.count);
      if (this.payloadType == null) {
        myPayload = this.payload;
      } else if (this.payloadType == "none") {
        myPayload = null;
      } else {
        myPayload = RED.util.evaluateNodeProperty(
          this.payload,
          this.payloadType,
          this,
          msg
        );
      }
      if (isNaN(myPayload)) {
        this.status({
          fill: "red",
          shape: "ring",
          text:
            "Payload type must be a number  (" +
            this.payload +
            ") value is missing or incorrect myPayload: (" +
            myPayload +
            ")",
        });
        return;
      } else {
        this.status({});
      }
      try {
        var hwAdd = DEFAULT_HW_ADD;

        if (myPayload < 0) {
          myPayload = 0;
        }
        if (myPayload > 10) {
          myPayload = 10;
        }
        var intVal = Math.round(myPayload * 1000);

        node.port.writeWord(
          hwAdd,
          I2C_MEM_U0_10_OUT_VAL1,
          intVal,
          function (err, size, res) {
            if (err) {
              node.error(err, msg);
            } else {
              //rmsg.payload = res.readIntLE(0, 2) / 1000.0;
              node.send(msg);
            }
          }
        );
      } catch (err) {
        this.error(err, msg);
      }
    });

    node.on("close", function () {
      node.port.closeSync();
    });
  }
  RED.nodes.registerType("LKIT 0-10V out", VOutNode);

  // current nodes

  function IInNode(n) {
    RED.nodes.createNode(this, n);
    this.payload = n.payload;
    this.payloadType = n.payloadType;
    var node = this;
    var buffer = Buffer.alloc(2);

    node.port = I2C.openSync(1);
    node.on("input", function (msg) {
      var myPayload;

      //var buffcount = parseInt(node.count);

      try {
        var hwAdd = DEFAULT_HW_ADD;

        if (this.payloadType == null) {
          myPayload = this.payload;
        } else if (this.payloadType == "none") {
          myPayload = null;
        } else {
          myPayload = RED.util.evaluateNodeProperty(
            this.payload,
            this.payloadType,
            this,
            msg
          );
        }
        node.port.readI2cBlock(
          hwAdd,
          I2C_MEM_I4_20_IN_VAL1,
          2,
          buffer,
          function (err, size, res) {
            if (err) {
              node.error(err, msg);
            } else {
              msg.payload = res.readIntLE(0, 2) / 1000.0;
              node.send(msg);
            }
          }
        );
      } catch (err) {
        this.error(err, msg);
      }
    });

    node.on("close", function () {
      node.port.closeSync();
    });
  }
  RED.nodes.registerType("LKIT 4-20mA in", IInNode);

  function IOutNode(n) {
    RED.nodes.createNode(this, n);
    this.payload = n.payload;
    this.payloadType = n.payloadType;
    var node = this;
    var buffer = Buffer.alloc(2);

    node.port = I2C.openSync(1);
    node.on("input", function (msg) {
      var myPayload;
      //var buffcount = parseInt(node.count);
      if (this.payloadType == null) {
        myPayload = this.payload;
      } else if (this.payloadType == "none") {
        myPayload = null;
      } else {
        myPayload = RED.util.evaluateNodeProperty(
          this.payload,
          this.payloadType,
          this,
          msg
        );
      }
      if (isNaN(myPayload)) {
        this.status({
          fill: "red",
          shape: "ring",
          text:
            "Payload type must be a number  (" +
            this.payload +
            ") value is missing or incorrect myPayload: (" +
            myPayload +
            ")",
        });
        return;
      } else {
        this.status({});
      }
      try {
        var hwAdd = DEFAULT_HW_ADD;

        if (myPayload < 4) {
          myPayload = 4;
        }
        if (myPayload > 20) {
          myPayload = 20;
        }
        var intVal = Math.round(myPayload * 1000);

        node.port.writeWord(
          hwAdd,
          I2C_MEM_I4_20_OUT_VAL1,
          intVal,
          function (err, size, res) {
            if (err) {
              node.error(err, msg);
            } else {
              //rmsg.payload = res.readIntLE(0, 2) / 1000.0;
              node.send(msg);
            }
          }
        );
      } catch (err) {
        this.error(err, msg);
      }
    });

    node.on("close", function () {
      node.port.closeSync();
    });
  }
  RED.nodes.registerType("LKIT 4-20mA out", IOutNode);

  function PWMOutNode(n) {
    RED.nodes.createNode(this, n);
    this.payload = n.payload;
    this.payloadType = n.payloadType;
    var node = this;
    var buffer = Buffer.alloc(2);

    node.port = I2C.openSync(1);
    node.on("input", function (msg) {
      var myPayload;

      //var buffcount = parseInt(node.count);
      if (this.payloadType == null) {
        myPayload = this.payload;
      } else if (this.payloadType == "none") {
        myPayload = null;
      } else {
        myPayload = RED.util.evaluateNodeProperty(
          this.payload,
          this.payloadType,
          this,
          msg
        );
      }
      if (isNaN(myPayload)) {
        this.status({
          fill: "red",
          shape: "ring",
          text:
            "Payload type must be a number  (" +
            this.payload +
            ") value is missing or incorrect myPayload: (" +
            myPayload +
            ")",
        });
        return;
      } else {
        this.status({});
      }
      try {
        var hwAdd = DEFAULT_HW_ADD;

        if (myPayload < -100) {
          myPayload = -100;
        }
        if (myPayload > 100) {
          myPayload = 100;
        }
        var intVal = Math.round(myPayload * 10);
        intVal = 0xffff & ((intVal << 16) >> 16); //Dimitry Soshnikov method
        node.port.writeWord(
          hwAdd,
          I2C_MEM_MOT_PWM1,
          intVal,
          function (err, size, res) {
            if (err) {
              node.error(err, msg);
            } else {
              //rmsg.payload = res.readIntLE(0, 2) / 1000.0;
              node.send(msg);
            }
          }
        );
      } catch (err) {
        this.error(err, msg);
      }
    });

    node.on("close", function () {
      node.port.closeSync();
    });
  }
  RED.nodes.registerType("LKIT MOT out", PWMOutNode);

  function OptoInNode(n) {
    RED.nodes.createNode(this, n);
    this.channel = parseInt(n.channel);
    this.payload = n.payload;
    this.payloadType = n.payloadType;
    this.inputs = n.inputs;
    this.bounce = parseInt(n.bounce);
    this.fetch = n.fetch;
    this.optoState = -1;
    this.lastState = -1;
    var node = this;

    if (node.fetch == "in") {
      var msg = { payload: Boolean(true) };
      if (node.bounce < 10) {
        node.bounce = 10;
      }
      if (node.bounce > 4000) {
        node.bounce = 4000;
      }
      var channel = node.channel;
      channel = parseInt(channel);
      if (isNaN(channel)) {
        this.status({
          fill: "red",
          shape: "ring",
          text: "Invalid [channel] value",
        });
        return;
      } else {
        this.status({});
      }

      if (channel < 1) {
        channel = 1;
      }
      if (channel > 4) {
        channel = 4;
      }

      rpio.open(gInPins[channel - 1], rpio.INPUT, rpio.PULL_UP);

      var intervalId = setInterval(function () {
        // try {
        var val = rpio.read(gInPins[channel - 1]);

        if (
          node.optoState !== -1 &&
          node.optoState !== val &&
          node.lastState == val
        ) {
          if (val == 0) {
            msg.payload = true;
          } else {
            msg.payload = false;
          }
          node.send(msg);
          node.optoState = val;
        }
        if (node.optoState == -1) {
          node.optoState = val;
        }
        node.lastState = val;
      }, node.bounce);
    } else {
      node.on("input", function (msg) {
        var myPayload;

        var channel = node.channel;
        if (isNaN(channel)) channel = msg.channel;
        channel = parseInt(channel);
        if (isNaN(channel)) {
          this.status({
            fill: "red",
            shape: "ring",
            text: "Invalid [msg.channel] value",
          });
          return;
        } else {
          this.status({});
        }
        if (channel > 4 || channel < 1) {
          this.status({
            fill: "red",
            shape: "ring",
            text: "invalid [msg.channel] value",
          });
          console.log("msg.channel value invalid, value: " + channel);
          return;
        }
        try {
          if (this.payloadType == null) {
            myPayload = this.payload;
          } else if (this.payloadType == "none") {
            myPayload = null;
          } else {
            myPayload = RED.util.evaluateNodeProperty(
              this.payload,
              this.payloadType,
              this,
              msg
            );
          }
          rpio.open(gInPins[channel - 1], rpio.INPUT, rpio.PULL_UP);
          var val = rpio.read(gInPins[channel - 1]);
          if (val) {
            msg.payload = false;
          } else {
            msg.payload = true;
          }
          node.send(msg);
        } catch (err) {
          this.error(err, msg);
        }
      });
    }
    node.on("close", function () {
      //node.port.closeSync();
    });
  }
  RED.nodes.registerType("LKIT OPTO in", OptoInNode);

  function RelayOutNode(n) {
    RED.nodes.createNode(this, n);

    this.relay = parseInt(n.relay);
    this.payload = n.payload;
    this.payloadType = n.payloadType;
    var node = this;

    node.on("input", function (msg) {
      var myPayload;

      var relay = node.relay;
      if (isNaN(relay)) relay = msg.relay;
      relay = parseInt(relay);
      if (isNaN(relay)) {
        this.status({
          fill: "red",
          shape: "ring",
          text: "Sensor number  (" + relay + ") value is missing or incorrect",
        });
        return;
      } else {
        this.status({});
      }

      if (relay < 1) {
        relay = 1;
      }
      if (relay > 2) {
        relay = 2;
      }
      try {
        if (this.payloadType == null) {
          myPayload = this.payload;
        } else if (this.payloadType == "none") {
          myPayload = null;
        } else {
          myPayload = RED.util.evaluateNodeProperty(
            this.payload,
            this.payloadType,
            this,
            msg
          );
        }
        rpio.open(gRelayPins[relay - 1], rpio.OUTPUT);
        var state = true;
        if (
          myPayload == null ||
          myPayload == false ||
          myPayload == 0 ||
          myPayload == "off"
        ) {
          state = false;
        }
        if (state) {
          rpio.write(gRelayPins[relay - 1], rpio.HIGH);
        } else {
          rpio.write(gRelayPins[relay - 1], rpio.LOW);
        }
      } catch (err) {
        this.error(err, msg);
      }
    });

    node.on("close", function () {
      
    });
  }
  RED.nodes.registerType("LKIT RELAY", RelayOutNode);

  function LedNode(n) {
    RED.nodes.createNode(this, n);
    this.led = parseInt(n.led);
    this.sel = n.sel;
    this.payload = n.payload;
    this.payloadType = n.payloadType;
    var node = this;

    node.port = I2C.openSync(1);
    node.on("input", function (msg) {
      var myPayload;

      var led = msg.led;
      if (isNaN(led)) led = node.led;
      led = parseInt(led);
      if (isNaN(led)) {
        this.status({
          fill: "red",
          shape: "ring",
          text: "Led number  (" + led + ") value is missing or incorrect",
        });
        return;
      } else {
        this.status({});
      }
      var hwAdd = DEFAULT_HW_ADD;

      try {
        if (this.payloadType == null) {
          myPayload = this.payload;
        } else if (this.payloadType == "none") {
          myPayload = null;
        } else {
          myPayload = RED.util.evaluateNodeProperty(
            this.payload,
            this.payloadType,
            this,
            msg
          );
        }
        //console.log("Led number " + led);
        //console.log("Select type " + node.sel);

        if (node.sel == 0) {
          //single led selection
          if (led > 4 || led < 1) {
            this.status({
              fill: "red",
              shape: "ring",
              text: "msg.led value invalid",
            });
            console.log("msg.led value invalid, value: " + led);
            return;
          }
          var val = node.port.readByteSync(hwAdd, I2C_MEM_LED_VAL_ADD);
          if (
            myPayload == false ||
            myPayload == 0 ||
            myPayload == "off" ||
            myPayload == "0"
          ) {
            val &= ~(1 << (led - 1));
          } else if (
            myPayload == true ||
            myPayload == 1 ||
            myPayload == "on" ||
            myPayload == "1"
          ) {
            val |= 1 << (led - 1);
          } else {
            this.status({
              fill: "red",
              shape: "ring",
              text: "msg.payload value invalid for single mode",
            });
            console.log(
              "msg.payload value invalid for single mode, value " + myPayload
            );
            return;
          }
          node.port.writeByteSync(hwAdd, I2C_MEM_LED_VAL_ADD, val);
          node.send(msg);
        } else {
          //group selection
          var relVal = parseInt(myPayload);
          if (isNaN(relVal) || relVal > 15 || relVal < 0) {
            this.status({
              fill: "red",
              shape: "ring",
              text: "msg.payload value invalid for group mode",
            });
            console.log(
              "msg.payload value invalid for group mode, value " + myPayload
            );
            return;
          } else {
            msg.payload = relVal;
            var ledVal = 0;
            ledVal = (0x01 & (relVal>>3)) + (0x02 & (relVal >> 1)) + (0x04 & (relVal << 1)) + (0x08 & (relVal << 3));
            node.port.writeByte(
              hwAdd,
              I2C_MEM_LED_VAL_ADD,
              ledVal,
              function (err) {
                if (err) {
                  node.error(err, msg);
                } else {
                  node.send(msg);
                }
              }
            );
          }
        }
      } catch (err) {
        this.error(err, msg);
      }
    });

    node.on("close", function () {
      node.port.closeSync();
    });
  }
  RED.nodes.registerType("LKIT LED", LedNode);

  function ButtonNode(n) {
    RED.nodes.createNode(this, n);
    this.bounce = parseInt(n.bounce);
    this.buttonState = -1;
    var node = this;
    var pin = 37;

    if (node.bounce < 10) {
      node.bounce = 10;
    }
    if (node.bounce > 4000) {
      node.bounce = 4000;
    }
    try {
      rpio.open(pin, rpio.INPUT, rpio.PULL_UP);

      var intervalId = setInterval(function () {
        var val = rpio.read(pin);
        if (node.buttonState !== -1 && node.buttonState !== val) {
          node.send({ payload: Number(/*1 ^*/ val) });
        }
        node.buttonState = val;
      }, node.bounce);
    } catch (err) {
      this.error(err);
    }

    node.on("close", function () {
      clearInterval(intervalId);
    });
  }

  RED.nodes.registerType("LKIT BUTTON", ButtonNode);
};
