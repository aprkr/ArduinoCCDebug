#define PIN_RESET 6
#define BAUD_RATE 115200
#define SPI_MHz 4 // Cannot exceed 12 MHz
#define wait625 {asm volatile("nop");} // This really isn't necessary since the Uno is never going to be too fast for the CC, but just in case anyways

// Serial commands
#define GET_CHIP_ID_SER 0x01
#define XDATAREAD_SER 0x02
