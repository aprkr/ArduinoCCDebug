#include <Arduino.h>
#include <config.h>
#include <SPI.h>
#include <cc254x.h>
#define disableMOSI (*portModeRegister(digitalPinToPort(MOSI)) &= ~digitalPinToBitMask(MOSI)) // Disable MOSI pin to not interfere with CC output
#define enableMOSI (*portModeRegister(digitalPinToPort(MOSI)) |= digitalPinToBitMask(MOSI)) // Same as DDRB |= (1<<DDB3)
#define readMISO (*portInputRegister(digitalPinToPort(MISO)) & digitalPinToBitMask(MISO) ? HIGH : LOW)

void startDebug() {
  pinMode(SCK, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  digitalWrite(SCK, LOW);
  digitalWrite(PIN_RESET,  HIGH);
  digitalWrite(PIN_RESET, LOW);
  wait625;
  wait625;
  wait625;
  digitalWrite(SCK, HIGH);
  wait625;
  digitalWrite(SCK, LOW);
  wait625;
  digitalWrite(SCK, HIGH);
  wait625;
  digitalWrite(SCK, LOW);
  wait625;
  wait625;
  digitalWrite(PIN_RESET, HIGH);
  wait625;
  wait625;
  SPI.begin();
  SPI.beginTransaction(SPISettings(SPI_MHz*1000000,MSBFIRST,SPI_MODE1));
  disableMOSI;
}

void DDWait() { // Sometimes need to wait for CC to be ready to send data (Figure 3-4) TODO make into macro
  // wait625;
  while (readMISO == HIGH) {
    SPI.transfer(0);
  }
}

void send(byte data) {
  enableMOSI;
  SPI.transfer(data);
  disableMOSI;
}

byte read() {
  return SPI.transfer(0);
}

void getChipID() {
  send(GET_CHIP_ID);
  DDWait();
  Serial.write(read());
  Serial.write(read());
}

void getRdConfig() {
  send(RD_CONFIG);
  DDWait();
  Serial.write(read());
}

void readStatus() {
  send(READ_STATUS);
  DDWait();
  Serial.write(read());
}

void setup() {
  Serial.begin(BAUD_RATE);
  startDebug();
}

void xDataRead(uint16_t address, uint16_t len) {
  enableMOSI;
  SPI.transfer(DEBUG_INSTR3);
  SPI.transfer(0x90);
  SPI.transfer16(address);
  disableMOSI;
  DDWait();
  read();
  for (uint16_t i = 0; i < len; i++) {
    enableMOSI;
    SPI.transfer(DEBUG_INSTR1);
    SPI.transfer(0xE0);
    disableMOSI;
    DDWait();
    byte bruh = read();
    Serial.write(bruh);

    enableMOSI;
    SPI.transfer(DEBUG_INSTR1);
    SPI.transfer(0xA3);
    disableMOSI;
    DDWait();
    read();
  }
  


}

byte sendInstr() {

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
    
    default:
      break;
    }
  }
}