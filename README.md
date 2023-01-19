
# CCLib


Implementation of Texas Instruments CC Debugger protocol on the Arduino Uno. Currently only supports the CC2543/4/5 chips, more support might be added in the future (see [CCLib](https://github.com/wavesoft/CCLib) for support of other CC chips). CCLib is an impressive project however, it didn't support my chips and was also extremely slow due to it using the Arduino as a proxy between the chip and the host PC. The proxy behavior is hindered by slow speed of serial communications between the PC and Arduino. This project also uses the SPI hardware built into the Arduino instead of bit-banging. The results are that flashing and verifying a 32kb CC2543 chip takes less than 10 seconds.

## Prepare your arduino board
Very similar setup to CCLib, resistors are used as voltage dividers since CC chips use 3.3V.


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

In an arduino/breadboard set-up, this looks like this:

