# mikroPLC

mikroPLC is a firmware for the Microchip's PIC18F4550 microcontroller which allows it to emulate the behaviour of a Programmable Logic Controller (aka PLC). Currently it supports 8 inputs and 4 outputs. The programs for the firmware are currently written as files containing a simplified version of Siemens Step7 Statement Lists.

The firmware is compiled using the CCS PICC Compiler 5.0 and has been tested on a physical PIC18F4550 PIC microcontroller as well as in a digital simulation on Proteus 8.7.

After loading the firmware, programs can be loaded into the Controller through a USB connection using the mikroPLC-Loader Python script that translates a plain text file containing the Siemens Step7 Statement List to a bytecode that the PIC will write in the internal EEPROM and interpret at runtime. There's also the possibility to use an external I2C EEPROM memory.
