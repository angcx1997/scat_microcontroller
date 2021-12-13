#!/usr/bin/env python3

import serial
import logging

logging.basicConfig(filename='control.log',level=logging.INFO,format='%(asctime)s:%(message)s')

print("Hello")
ser = serial.Serial('/dev/ttyACM0', baudrate=115200)

while(1):
    ser_bytes = ser.readline()
    logging.info(ser_bytes)


