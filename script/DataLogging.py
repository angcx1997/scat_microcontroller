#!/usr/bin/env python3

import serial
import logging

def bytes_to_int(byte1, byte2):
    result = 0
    bytes = (byte1<<8) | byte2

    for b in bytes:
        result = result * 256 + int(b)

    return result


logging.basicConfig(filename='control.log',level=logging.INFO,format='%(asctime)s : %(message)s')

print("Hello")
ser = serial.Serial('/dev/ttyACM0', baudrate=115200)

receive_size = 2 * 7
count = 0
byte1 = 0
byte2 = 0

start_byte1 = b'\xFB'
start_byte2 = b'\xFB'

end_byte2 = b'\xAB'
end_byte1 = b'\xCD'

checkflag = 0
endflag = 0
while(1):
    ser_bytes = ser.read()
    # print(ser_bytes)
    if (ser_bytes == start_byte1 and checkflag == 0):
        checkflag = 1
        count += 1
    if (ser_bytes == start_byte2 and checkflag == 1):
        checkflag = 2
        count += 1

    # 4 is start and end byte
    if (checkflag == 2 and count < receive_size - 4):
        if (count%2 == 1):
            byte1 = ser_bytes
        else:
            byte2 = ser_bytes
            t_byte = byte1 + byte2
            print(int.from_bytes(t_byte, "big"))
        count+=1
    
    if (checkflag == 2 and ser_bytes == end_byte1):
        endflag = 1
    if (checkflag == 2 and endflag == 1 and ser_bytes == end_byte2):
        endflag =2
        checkflag = 0
        count = 0

    # logging.info(hex(ser_bytes))