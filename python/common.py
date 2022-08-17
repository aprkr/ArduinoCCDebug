#!/usr/bin/env python

import serial, time
from serial.tools import list_ports

GET_CHIP_ID_SER = 0x01

def connectArduino():
    comport = None
    search = 'USB VID:PID=2341:0043'.lower()
    for port in list_ports.comports():
        if search in port[2].lower():
            comport = port[0]
        break
    if not comport:
        raise Exception('Failed to find Arduino COM port.')
    ser = serial.Serial(port=comport, baudrate=115200, timeout=5)
    time.sleep(2)
    return ser

def getChipId(ser : serial.Serial) -> bytes:
    ser.write(bytearray([GET_CHIP_ID_SER]))
    return ser.read(2)