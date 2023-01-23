#include <Arduino.h>
#include <config.h>
#include <SPI.h>
#include <cc254x.h>
#define disableMOSI (*portModeRegister(digitalPinToPort(MOSI)) &= ~digitalPinToBitMask(MOSI)) // Disable MOSI pin to not interfere with CC output
#define enableMOSI (*portModeRegister(digitalPinToPort(MOSI)) |= digitalPinToBitMask(MOSI)) // Same as DDRB |= (1<<DDB3)
#define readMISO (*portInputRegister(digitalPinToPort(MISO)) & digitalPinToBitMask(MISO))

void startDebug() { // 2 falling-edges on clock pin when reset is low, then set reset high
  pinMode(SCK, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  digitalWrite(PIN_RESET, HIGH);
  digitalWrite(PIN_RESET, LOW);
  delayMicroseconds(1); // All these waits are excessive, but harmless
  digitalWrite(SCK, HIGH);
  delayMicroseconds(1);
  digitalWrite(SCK, LOW);
  delayMicroseconds(1);
  digitalWrite(SCK, HIGH);
  delayMicroseconds(1);
  digitalWrite(SCK, LOW);
  delayMicroseconds(1);
  digitalWrite(PIN_RESET, HIGH);
  delayMicroseconds(1);
  SPI.begin();
  SPI.beginTransaction(SPISettings(SPI_MHz*1000000,MSBFIRST,SPI_MODE1));
  disableMOSI; // Keep MOSI disabled all the time unless sending data to CC
}

void DDWait() { // Sometimes need to wait for CC to be ready to send data (Figure 3-4)
  while (readMISO) {
    SPI.transfer(0);
  }
}

void sendToCC(byte data) {
  enableMOSI;
  SPI.transfer(data);
  disableMOSI;
}

byte readCC() {
  return SPI.transfer(0);
}

byte sendInstr1(byte instr) {
  enableMOSI;
  SPI.transfer(DEBUG_INSTR1);
  SPI.transfer(instr);
  disableMOSI;
  DDWait();
  return readCC();
}

byte sendInstr2(byte instr, byte operand) {
  enableMOSI;
  SPI.transfer(DEBUG_INSTR2);
  SPI.transfer(instr);
  SPI.transfer(operand);
  disableMOSI;
  DDWait();
  return readCC();
}

byte sendInstr3(byte instr, uint16_t operand) {
  enableMOSI;
  SPI.transfer(DEBUG_INSTR3);
  SPI.transfer(instr);
  SPI.transfer16(operand);
  disableMOSI;
  DDWait();
  return readCC();
}

byte sendInstr3(byte instr, byte operand1, byte operand2) {
  enableMOSI;
  SPI.transfer(DEBUG_INSTR3);
  SPI.transfer(instr);
  SPI.transfer(operand1);
  SPI.transfer(operand2);
  disableMOSI;
  DDWait();
  return readCC();
}

void getChipID() {
  sendToCC(GET_CHIP_ID);
  DDWait();
  Serial.write(readCC());
  Serial.write(readCC());
}

void getRdConfig() {
  sendToCC(RD_CONFIG);
  DDWait();
  Serial.write(readCC());
}

void readStatus() {
  sendToCC(READ_STATUS);
  DDWait();
  Serial.write(readCC());
}

void xDataRead(uint16_t address, uint16_t len) {
  sendInstr3(0x90, address); // MOV DPTR, #data16
  for (uint16_t i = 0; i < len; i++) {
    Serial.write(sendInstr1(0xE0)); // MOVX A, @DPTR
    sendInstr1(0xA3); // INC DPTR
  }
}

void xDataWrite(uint16_t address, byte len, byte *buf) {
  sendInstr3(0x90, address); // MOV DPTR, #data16
  for (byte i = 0; i < len; i++) {
    sendInstr2(0x74, buf[i]); // MOV A, #data
    sendInstr1(0xF0); // MOVX @DPTR, A
    sendInstr1(0xA3); // INC DPTR
  }
}

void writeChip() {
  // Erase chip
  sendToCC(CHIP_ERASE);
  DDWait();
  readCC();
  do {
    delay(10); // Mass erase time is 20 ms
    sendToCC(READ_STATUS);
    DDWait();
  } while ((readCC() & 0b10000000) == 0b10000000);
  
  for (byte page = 0; page < 32; page++) {
    for (byte i = 0; i < 2; i++) {
      // Enable DMA
      enableMOSI;
      sendToCC(WR_CONFIG);
      sendToCC(0b00100010);
      disableMOSI;
      DDWait();
      readCC();

      // DMA to send DBGDATA to some 0x0000 XDATA, triggered by DBG_BW
      byte buf [8] = {0x62, 0x60, 0x00, 0x00, 0b00000010, 0b00000000, 0b00011111, 0b00011010};
      xDataWrite(0x0200, 8, buf);

      sendInstr3(0x75, DMAIRQ, 0x00); // MOV direct, #data

      sendInstr3(0x75, DMAARM, 0x00); // MOV direct, #data

      // Point DMA config bytes
      sendInstr3(0x75, DMA1CFGL, 0x00); // MOV direct, #data

      sendInstr3(0x75, DMA1CFGH, 0x02); // MOV direct, #data

      // Arm DMA
      sendInstr3(0x75, DMAARM, 0b10); // MOV direct, #data
      // DMA channel arming takes 9 clocks, however sending data takes more than 9 clocks anyways so no need to wait
      enableMOSI;
      SPI.transfer(BURST_WRITE | 0b010);
      SPI.transfer(0);
      for (byte j = 0; j < 512 / ARD_BUFF_SIZE; j++) {
        while (Serial.available() < ARD_BUFF_SIZE) {}
        for (byte k = 0; k < ARD_BUFF_SIZE; k++) {
          SPI.transfer(Serial.read());
        }
        Serial.write((byte)0x1);
      }
      disableMOSI;
      DDWait();
      readCC();
      // Wait til DMA is done
      while ((sendInstr2(0xE5, 0xD6) & 00000010) == 1);
      // Configure DMA channel to send 512 bytes to FWDATA from XDATA, triggered by FLASH
      byte buf2 [8] = {0x00, 0x00, 0x62, 0x73, 0b00000010, 0b00000000, 0b00010010, 0b01001010};
      xDataWrite(0x0200, 8, buf2);

      sendInstr3(0x75, DMAIRQ, 0x00); // MOV direct, #data

      // Set flash word address (32-bit words)
      sendInstr3(0x90, 0x6271); // MOV DPTR, #data16
      uint16_t wordAddress = (page * 1024 + i * 512) / 4;
      sendInstr2(0x74, (wordAddress & 0xFF)); // MOV A, #data
      sendInstr1(0xF0); // MOVX @DPTR, A
      sendInstr1(0xA3); // INC DPTR
      sendInstr2(0x74, ((wordAddress >> 8) & 0xFF)); // MOV A, #data
      sendInstr1(0xF0); // MOVX @DPTR, A
      // Arm DMA
      sendInstr3(0x75, DMAARM, 0b10); // MOV direct, #data
      // Set FCTL.WRITE to 1
      sendInstr3(0x90, 0x6270); // MOV DPTR, #data16
      sendInstr2(0x74, 0b00000010); // MOV A, #data
      sendInstr1(0xF0); // MOVX @DPTR, A
      // Wait until FCTL.BUSY = 0
      while (sendInstr1(0xE0) & 0b10000000) { // MOVX A, @DPTR
        delayMicroseconds(1);
      }
    }
  }
}

void setup() {
  Serial.begin(BAUD_RATE);
  startDebug();
}

void loop() {
  if (Serial.available()) {
    byte command = Serial.read();
    switch (command)
    {
    case GET_CHIP_ID_SER: {
      getChipID();
      break;
    }
    case XDATAREAD_SER: {
      while (Serial.available() < 4) {}
      uint16_t addr = Serial.read() << 8;
      addr |= Serial.read();
      uint16_t len = Serial.read() << 8;
      len |= Serial.read();
      xDataRead(addr, len);
      break;
    }
    case WRITE_CHIP_SER: {
      writeChip();
      break;
    }
    case RD_CONFIG_SER: {
      sendToCC(RD_CONFIG);
      DDWait();
      Serial.write(readCC());
      break;
    }
    case READ_STATUS_SER: {
      sendToCC(READ_STATUS);
      DDWait();
      Serial.write(readCC());
      break;
    }
    case STEP_SER: {
      sendToCC(GET_PC);
      DDWait();
      Serial.write(readCC());
      Serial.write(readCC());
      sendToCC(STEP_INSTR);
      DDWait();
      Serial.write(readCC());
      break;
    }
    
    default:
      break;
    }
  }
}