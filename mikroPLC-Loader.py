# -*- coding: cp1252 -*-
import re
from msvcrt import kbhit
import easygui as gui
import pywinusb.hid as hid

from pywinusb import *

in_data = []

program_bytes = []

def readData(data):
    if data[1] != 0xFF:
        in_data.append( hex(data[1]) + '\n')
    else:
        gui.codebox("Code Received.", "mikroPLC-Loader", in_data)
        exit()
    
def load_to_usb():
    filter_micro = hid.HidDeviceFilter(vendor_id=0x0461, product_id=0x0020)
    devices = filter_micro.get_devices()

    try:
        micro_hid = devices[0]
        gui.msgbox("USB Successfully Connected.", "mikroPLC-Loader")
    except IndexError:
        gui.msgbox("Device not detected.", "mikroPLC-Loader")
        exit()

    micro_hid.open()
    micro_hid.set_raw_data_handler(readData)
    reports = micro_hid.find_output_reports()

    buffer = [0xFF]*3
    buffer[0] = 0x00

    buffer[1]=0x33
    reports[0].set_raw_data(buffer)
    reports[0].send()

    for data_byte in program_bytes:
        buffer[1]=data_byte
        reports[0].set_raw_data(buffer)
        reports[0].send()

    buffer[1]=0xFF
    reports[0].set_raw_data(buffer)
    reports[0].send()

    buffer[1]=0xFF
    reports[0].set_raw_data(buffer)
    reports[0].send()

    micro_hid.close()

    gui.msgbox("Program Loaded To PIC Correctly", "mikroPLC-Loader")

def download_from_usb():
    filter_micro = hid.HidDeviceFilter(vendor_id=0x0461, product_id=0x0020)
    devices = filter_micro.get_devices()

    try:
        micro_hid = devices[0]
        gui.msgbox("USB Successfully Connected.", "mikroPLC-Loader")
    except IndexError:
        gui.msgbox("Device not detected.", "mikroPLC-Loader")
        exit()

    micro_hid.open()
    micro_hid.set_raw_data_handler(readData)

    reports = micro_hid.find_output_reports()

    buffer = [0xFF]*3
    buffer[0] = 0x00
    buffer[1]=0x22
    reports[0].set_raw_data(buffer)
    reports[0].send()
    
    micro_hid.close()
    micro_hid.open()
    
    input_reports = []

    while True:
        if kbhit():
            input_reports = micro_hid.find_input_reports()

    micro_hid.close()

ok=gui.msgbox("Welcome mikroPLC-Loader.","mikroPLC-Loader", ok_button="Start")

if ok == "Start":
    pass
else:
    exit()

selection = gui.buttonbox("Choose what you want to do:",
                          "mikroPLC-Loader",
                          ("Load Code to mikroPLC",
                           "Download code from mikroPLC") )

if selection == "Download code from mikroPLC":
    download_from_usb()
elif selection == "Load Code to mikroPLC":
    pass
else:
    exit()

ok=gui.msgbox("Now select the .txt file " +
              "that contains the AWL Code from Step-7 that you want to load to the PIC.",
           "mikroPLC-Loader")

if ok == "OK":
    pass
else:
    exit()

####Looking for the Statement List File#####
extension = ["*.txt"]   #.txt files by default

code_filename = gui.fileopenbox("",
                                "Select the code file with the " +
                                "statement list...",
                                "",extension) 
code_file = open(code_filename, "r")
code_file_text = code_file.readlines()
code_file.close()

modified_code = gui.codebox("Verify the AWL Code.",
                            "mikroPLC-Loader: " + code_filename, code_file_text)

if modified_code == None:
    exit()

code_file = open(code_filename, "w")
code_file.write(modified_code)
code_file.close()

code_file = open(code_filename, "r")
code_file_text = code_file.readlines()
code_file.close()

####Bytecode file generation#####

commands_txt = ["LD", "LDN", "A", "AN", "ALD", "O", "ON", "OLD", "="]
symbols_txt = ["I1", "I2", "I3", "I4", "I5", "I6", "I7", "I8", "Q1", "Q2",
               "Q3", "Q4", "T0"]

cur_command = cur_symbol = instr = 0

for line in code_file_text:
    for command in commands_txt:
        if re.match(command+"\s", line):
            cur_command = commands_txt.index(command)
            print "Command:\t" + command
    for symbol in symbols_txt:
        if re.search(symbol, line):
            cur_symbol = symbols_txt.index(symbol)
            print "Symbol:\t" + symbol

    instr = cur_command << 4
    if cur_command != 4 and cur_command != 7:
        instr = instr | cur_symbol

    program_bytes.append(instr)

    print "Hex:\t" + hex(instr)
    print "--------------------"

bin_filename = code_filename.strip(".txt") + ".bin"
bin_file = open(bin_filename, "wb")

bin_file.write(bytearray(program_bytes))

bin_file.close()

####Configuration of USB communication for transmission####
load_to_usb()
