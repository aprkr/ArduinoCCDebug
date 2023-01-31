#define PIN_RESET 6
#define BAUD_RATE 115200
#define SPI_MHz 8 // Cannot exceed 12 MHz
#define ARD_BUFF_SIZE 32

// Serial commands
#define GET_CHIP_ID_SER 1
#define XDATAREAD_SER 2
#define WRITE_CHIP_SER 3
#define RD_CONFIG_SER 4
#define READ_STATUS_SER 5
#define STEP_SER 6
#define DETECT_SER 7
#define DETECT_SER_RESPONSE 0xAB

#ifdef UNO
#define disableMOSI (*portModeRegister(digitalPinToPort(MOSI)) &= ~digitalPinToBitMask(MOSI)) // Disable MOSI pin to not interfere with CC output
#define enableMOSI (*portModeRegister(digitalPinToPort(MOSI)) |= digitalPinToBitMask(MOSI)) // Same as DDRB |= (1<<DDB3)
#define readMISO (*portInputRegister(digitalPinToPort(MISO)) & digitalPinToBitMask(MISO))
#elif PICO
#define disableMOSI (void)0
#define enableMOSI (void)0
#define DD_WAIT_PIN 7
#define readMISO digitalRead(DD_WAIT_PIN) // Reading MISO doesn't work for some reason, I'm guessing since it's being used in SPI
#endif
