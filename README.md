
# Arduino CC Debug


Implementation of Texas Instruments CC Debugger protocol on the Arduino Uno. Currently only supports the CC2543/4/5 chips, more support might be added in the future (see [CCLib](https://github.com/wavesoft/CCLib) for support of other CC chips). CCLib is an impressive project however, it didn't support my chips and was also extremely slow due to it using the Arduino as a proxy between the chip and the host PC. The proxy behavior is hindered by slow speed of serial communications between the PC and Arduino. This project also uses the SPI hardware built into the Arduino instead of bit-banging. The results are that flashing and verifying a 32kb CC2543 chip takes less than 10 seconds with an Arduino Uno.

## Arduino Framework

This project uses the arduino framework via platformio. In theory, this project should work on any board listed [here](https://docs.platformio.org/en/stable/frameworks/arduino.html#boards), however only an Arduino Uno and Raspberry Pi Pico have been tested to work. Adjust platformio.ini and config.h to add your own board. Also keep in mind that hardware SPI is used rather than bit-banging.

## Prepare your board

### Arduino Uno

Very similar setup to CCLib, resistors are used as voltage dividers since CC chips take 3.3V.


```
For the DD Pin:

 MOSI (11)--[ 10k ]-- MISO (12)--[ 20k ]-- <GND>
                        |   
                       {DD}

For the DC Pin:

 SCK (13)--[ 10k ]-- {DC} --[ 20k ]-- <GND>

For the RST Pin:

 <CC_RST> (6 by default) --[ 10k ]-- {RST} --[ 20k ]-- <GND>
```

Where `{DD}`, `{DC}` and `{RST}` are the pins on the CCxxxx chip and `MOSI`, `MISO`, `SCK`, `<CC_RST>` are the pins in your ardiuno board.

### Raspberry Pi Pico

No need for voltage divider since pico outputs only 3.3V anyways. However a resistor is needed between MOSI and MISO since I couldn't disable MOSI in software.

```
For the DD Pin:

 MOSI (19)--[ 10k ]-- MISO (16)-- {DD}
                        |   
                <DD_WAIT_PIN> (7 by default)

For the DC Pin:

 SCK (18) -- {DC}

For the RST Pin:

 <CC_RST> (6 by default) -- {RST}
```

Where `{DD}`, `{DC}` and `{RST}` are the pins on the CCxxxx chip and `MOSI`, `MISO`, `SCK`, `<CC_RST>`, `<DD_WAIT_PIN>` are the pins in your pico.
## Software

1. Tested with Python 3.10, pyserial 3.5, and platformio 6.1
2. Install firmware to Arduino device with platformio:

```
pio run -e (desired environment in platformio.ini) -t upload
```