#!/usr/bin/env python

from common import *

arg_parser = argparse.ArgumentParser( description="Read flash from CC254x" )
arg_parser.add_argument("path_to_output_file")
arguments = arg_parser.parse_args()
file = arguments.path_to_output_file

ser = connectArduino()
dump = readXData(ser, FLASH_START, FLASH_SIZE)
with open(file, "wb") as binary_file:
    binary_file.write(dump)