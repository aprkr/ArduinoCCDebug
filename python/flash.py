#!/usr/bin/env python

from common import *

arg_parser = argparse.ArgumentParser( description="Flash binary file to CC254x" )
arg_parser.add_argument("path_to_input_file")
arguments = arg_parser.parse_args()
file = arguments.path_to_input_file

ser = connectArduino()
with open(file, "rb") as binary_file:
    data = bytearray(binary_file.read())
    currAddress = 0
    sendByte(ser, WRITE_CHIP_SER)
    for page in range(NUM_PAGES):
        for x in range(int(PAGE_SIZE / ARD_BUFF_SIZE)):
            for y in range(ARD_BUFF_SIZE):
                sendByte(ser, data[currAddress])
                currAddress += 1
            ser.read(1)
    read = readXData(ser, FLASH_START, FLASH_SIZE)
    success = True
    for page in range(NUM_PAGES):
        if (read[page*PAGE_SIZE:(page+1)*PAGE_SIZE] != data[page*PAGE_SIZE:(page+1)*PAGE_SIZE]):
            success = False
            print("Data mismatch at page: " + str(page))
    if (success):
        print("Flash successful")
    else:
        print("Flash failed")