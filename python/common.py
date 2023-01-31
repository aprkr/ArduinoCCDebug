#!/usr/bin/env python

import serial, time
from serial import Serial
from serial.tools import list_ports
import argparse

GET_CHIP_ID_SER = 1
XDATAREAD_SER = 2
WRITE_CHIP_SER = 3
RD_CONFIG_SER = 4
READ_STATUS_SER = 5
STEP_SER = 6
DETECT_SER = 7
DETECT_SER_RESPONSE = 0xAB # I just chose some arbitrary number

ARD_BUFF_SIZE = 32
NUM_PAGES = 32
PAGE_SIZE = 1024
FLASH_SIZE = 0x8000
FLASH_START = 0x8000
BAUD_RATE = 115200

def connectArduino():
    comport = None
    for port in list_ports.comports():
        comport = port[0]
        ser = serial.Serial(port=comport, baudrate=BAUD_RATE, timeout=2)
        sendByte(ser, DETECT_SER)
        response = ser.read(1)
        if response == DETECT_SER_RESPONSE:
            break
    if not comport:
        raise Exception('Failed to find Arduino COM port.')
    return ser

def sendByte(ser: Serial, byte):
    ser.write(bytearray([byte]))

def sendBytes(ser: Serial, bytes):
    ser.write(bytearray(bytes))

def getChipId(ser : Serial) -> bytes:
    sendByte(ser, GET_CHIP_ID_SER)
    return ser.read(2)

def rdConfig(ser: Serial) -> bytes:
    sendByte(ser, RD_CONFIG_SER)
    return ser.read(1)

def readStatus(ser: Serial) -> bytes:
    sendByte(ser, READ_STATUS_SER)
    return ser.read(1)

def readXData(ser : Serial, address: int, length) -> bytes:
    sendBytes(ser, [XDATAREAD_SER, (address >> 8) & 0xFF, address & 0xFF, (length >> 8) & 0xFF, length & 0xFF])
    data = bytearray(length)
    for x in range(length):
        data[x] = ser.read(1)[0]
    return data
