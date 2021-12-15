#!/usr/bin/env python3

import csv

input_file = open("control.log",'r')
output_csv = open("control.csv",'w')

for line in input_file.readlines():
    print(line.split())