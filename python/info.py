#!/usr/bin/env python

# import common, serial
from common import *

ser = connectArduino()
print(getChipId(ser).hex())

