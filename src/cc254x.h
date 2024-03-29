#define CHIP_ERASE 0b00010000
#define WR_CONFIG 0b00011000
#define RD_CONFIG 0b00100000
#define GET_PC 0b00101000
#define READ_STATUS 0b00110000
#define SET_HW_BRKPNT 0b00111000
#define HALT 0b01000000
#define RESUME 0b01001000
#define DEBUG_INSTR1 0b01010001
#define DEBUG_INSTR2 0b01010010
#define DEBUG_INSTR3 0b01010011
#define STEP_INSTR 0b01011000
#define GET_BM 0b01100000 // Only available on CC253x and CC2540/41
#define GET_CHIP_ID 0b01101000
#define BURST_WRITE 0b10000000

#define DMAIRQ 0xD1
#define DMA1CFGL 0xD2
#define DMA1CFGH 0xD3
#define DMAARM 0xD6