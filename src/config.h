#define PIN_RESET 6
#define BAUD_RATE 115200
#define SPI_MHz 8 // Cannot exceed 12 MHz
#define waitCCcycle {asm volatile("nop");} // This really isn't necessary since the Uno is never going to be too fast for the CC, but just in case anyways
#define ARD_BUFF_SIZE 32

// Serial commands
#define GET_CHIP_ID_SER 1
#define XDATAREAD_SER 2
#define WRITE_CHIP_SER 3
#define RD_CONFIG_SER 4
#define READ_STATUS_SER 5
#define STEP_SER 6
