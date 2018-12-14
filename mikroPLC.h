////////////////////////////////////////////////////////////////////////////
////                         mikroPLC.h                                 ////
////                                                                    ////
//// Main Configuration of the mikroPLC firmware.                       ////
////////////////////////////////////////////////////////////////////////////
////              (C) Copyright 2018 Willians Briceño                   ////
////////////////////////////////////////////////////////////////////////////

#include <18F4550.h>

#fuses   NOWDT, NOPROTECT, NOBROWNOUT, NOLVP, NOPUT, NODEBUG  
#fuses   NOCPD, NOCPB, NOWRT, NOWRTC, NOWRTD, NOEBTR, NOEBTRB   
#fuses   HSPLL, USBDIV, PLL4, CPUDIV1, VREGEN
         
#use delay(clock=48MHz)
#use fast_io(a)
#use fast_io(d)

////////////////////////////////////I2C////////////////////////////////////////
#define I2C_SDA   PIN_B0
#define I2C_SCL   PIN_B1

#use i2c(master, sda=I2C_SDA, scl=I2C_SCL)

////////////////////////////////////USB////////////////////////////////////////
#define USB_CONFIG_HID_TX_SIZE 2
#define USB_CONFIG_HID_RX_SIZE 2

#include <pic18_usb.h>
#include <usb_desc_hid.h>
#include <usb.c>

///////////////////////////////PLC Input Pins//////////////////////////////////
#define PLC_I1          PIN_D0
#define PLC_I2          PIN_D1
#define PLC_I3          PIN_D2
#define PLC_I4          PIN_D3
#define PLC_I5          PIN_D4
#define PLC_I6          PIN_D5
#define PLC_I7          PIN_D6
#define PLC_I8          PIN_D7

//////////////////////////////PLC Output Pins//////////////////////////////////
#define PLC_Q1          PIN_A0
#define PLC_Q2          PIN_A1
#define PLC_Q3          PIN_A2
#define PLC_Q4          PIN_A3

/////////////////////////////////PLC Menu//////////////////////////////////////
#define UP              PIN_B7
#define DOWN            PIN_B6
#define ENTER           PIN_B5
#define BACK            PIN_B4

#define NUMBER_OF_MENU_OPTIONS   6

/////////////////////////////////Commands//////////////////////////////////////
#define CMD_LD    0x0
#define CMD_LDN   0x1
#define CMD_A     0x2
#define CMD_AN    0x3
#define CMD_ALD   0x4
#define CMD_O     0x5
#define CMD_ON    0x6
#define CMD_OLD   0x7
#define CMD_OUT   0x8

//////////////////////////////////Delays///////////////////////////////////////
#define DELAY_MESSAGE_MS         2500
#define DELAY_BUTTON_PRESS_MS    500
#define DELAY_SEND_BYTE_TO_PC    50

#define DELAY_MEMORY_WRITE_MS    40
#define DELAY_MEMORY_READ_MS     1

#define DELAY_I2C                1

//////////////////////////////////EEPROM///////////////////////////////////////

#ifndef USE_EXT_EEPROM
   #include "internal_eeprom.c"
#else
   #ifdef USE_2404
      #include "2404.c"
   #endif
#endif

////////////////////////////////////LCD////////////////////////////////////////
#define LCD_RS_PIN      PIN_A4
#define LCD_RW_PIN      PIN_A5
#define LCD_ENABLE_PIN  PIN_E0
#define LCD_DATA4       PIN_E1
#define LCD_DATA5       PIN_E2
#define LCD_DATA6       PIN_C0
#define LCD_DATA7       PIN_C1

#ifdef USE_LCD_I2C
   #include "lcd_i2c.c"
#else
   #include <lcd.c>
#endif

/////////////////////////////Menu Variables////////////////////////////////////
const char MENU_OPTIONS[6][25] = {">Ejecutar\n Programa", ">Revisar\n Instrucciones", ">Conectar a PC", ">Cargar\n Ejemplo 1", ">Cargar\n Ejemplo 2", ">Borrar\n Programa"};

///////////////////////////LD List Variables///////////////////////////////////
#ifdef USE_2404
   int1 list_LD[512] = {};
   int16 cur_LD_pos = 0;
#else
   int1 list_LD[256] = {};
   int8 cur_LD_pos = 0;
#endif                                                                        

//////////////////////////Commands & Symbols///////////////////////////////////
int8 symbols[12] = { PLC_I1, PLC_I2, PLC_I3, PLC_I4, PLC_I5, PLC_I6, PLC_I7, 
                     PLC_I8, PLC_Q1, PLC_Q2, PLC_Q3, PLC_Q4};
                     
const char commands_text[9][4] = {"LD", "LDN", "A", "AN", "ALD", "O", "ON", "OLD", "="};
const char symbols_text[13][3] = {"I1", "I2", "I3", "I4", "I5", "I6", "I7", "I8", "Q1",
                                 "Q2", "Q3", "Q4", "T0"};
                                                  
int8 plc_get_command(int8 instr);
int8 plc_get_symbol(int8 instr);

void exec_ld(int1 val);
void exec_ldn(int1 val);
void exec_a(int1 val);
void exec_an(int1 val);
void exec_ald();
void exec_o(int1 val);
void exec_on(int1 val);
void exec_old();
void exec_out(int8 output);

void plc_process_instr(int8 instr);

void msg_loading();
void msg_writing_succesfull();
void msg_no_program();

void plc_write_example();
void plc_write_example_2();

void plc_run_code();
void plc_show_line(EEPROM_ADDRESS address);
void plc_review_code();

void plc_connect_to_pc();

void plc_erase();

void show_menu_option();
void enter_menu_option();
