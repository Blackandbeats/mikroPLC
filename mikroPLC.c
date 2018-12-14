////////////////////////////////////////////////////////////////////////////
////                         mikroPLC.c                                 ////
////                                                                    ////
//// Main file of the source code for the mikroPLC firmware.            ////
////////////////////////////////////////////////////////////////////////////
////                                                                    ////
////  Version History:                                                  ////
////                                                                    ////
////  Oct 24th, 2018:                                                   ////
////     First Version                                                  ////
////                                                                    ////
////  Oct 28th, 2018:                                                   ////
////    -Subroutine plc_conect_to_pc() rewritten to work with USB HID   ////
////     class instead of USB CDC.                                      ////
////    -BACK button enabled in the USB menu.                           ////
////    -Method usb_init() call moved to main project file.             ////
////    -plc_connect_to_pc extended to allow sending the code in the    ////
////     memory to the PC.                                              ////
////                                                                    ////
////  Oct 30th, 2018:                                                   ////
////     -plc_run_code modified to dump the code into the RAM to work   ////
////      with the loaded_code array instead of reading the EEPROM      ////
////      continuously.                                                 ////
////     -Routine for proccessing of instructions modified to work      ////
////      with a switch-case structure instead of if-else.              ////
////                                                                    ////
////  Oct 30th, 2018:                                                   ////
////     -EEPROM and I2C configuration definitions were moved here      ////
////      from the header file.                                         ////
////                                                                    ////
////  Nov 2nd, 2018:                                                    ////
////     -plc_get_command_text and plc_get_symbol_text removed, instead ////
////      text values in the commands_text and symbols_text arrays are  ////
////      accesed directly by index.                                    ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////
////              (C) Copyright 2018 Willians Briceño                   ////
////////////////////////////////////////////////////////////////////////////

////////////////////////////Header Configuration///////////////////////////////

//#define  USE_EXT_EEPROM

//#define  USE_2404

//#define USE_LCD_I2C

#include <mikroPLC.h>

/////////////////////////////Info Getters//////////////////////////////////////
int8 plc_get_command(int8 instr)       { return instr >> 4; }

int8 plc_get_symbol(int8 instr)        { return instr & 0xF; }

///////////////////////////Command Execution///////////////////////////////////
void exec_ld(int1 val)  { list_LD[cur_LD_pos] = val; cur_LD_pos++; }

void exec_ldn(int1 val) { list_LD[cur_LD_pos] = !val; cur_LD_pos++; }

void exec_a(int1 val)   { list_LD[cur_LD_pos-1] = (list_LD[cur_LD_pos-1] & val); }

void exec_an(int1 val)  { list_LD[cur_LD_pos-1] = (list_LD[cur_LD_pos-1] & !val); }

void exec_ald()         { list_LD[cur_LD_pos-2] = (list_LD[cur_LD_pos-1]) & (list_LD[cur_LD_pos-2]); cur_LD_pos--; }

void exec_o(int1 val)   { list_LD[cur_LD_pos-1] = list_LD[cur_LD_pos-1] | val; }

void exec_on(int1 val)  { list_LD[cur_LD_pos-1] = list_LD[cur_LD_pos-1] | !val; }

void exec_old()         { list_LD[cur_LD_pos-2] = (list_LD[cur_LD_pos-1]) | (list_LD[cur_LD_pos-2]); cur_LD_pos--; }

void exec_out(int8 output){
   if(output > 7 && output < 12){
      if(list_LD[0] == 0)
         output_low(symbols[output]);
      else if(list_LD[0] == 1)
         output_high(symbols[output]);
   }
    
   cur_LD_pos = 0;
}

/////////////////////////Instruction Proccessing///////////////////////////////
void plc_process_instr(int8 instr){
   int8 command = plc_get_command(instr);
   int8 symbol = plc_get_symbol(instr);
   
   switch(command){
      case CMD_LD:   exec_ld(input(symbols[symbol]));    break;
      case CMD_LDN:  exec_ldn(input(symbols[symbol]));   break;
      case CMD_A:    exec_a(input(symbols[symbol]));     break;
      case CMD_AN:   exec_an(input(symbols[symbol]));    break;
      case CMD_ALD:  exec_ald();                         break;
      case CMD_O:    exec_o(input(symbols[symbol]));     break;
      case CMD_ON:   exec_on(input(symbols[symbol]));    break;
      case CMD_OLD:  exec_old();                         break;
      case CMD_OUT:  exec_out(symbol);                   break;
   }
   
}

//////////////////////////////////Messages/////////////////////////////////////
void msg_loading(){
   printf(lcd_putc, "\fCargando...");
}

void msg_writing_succesfull(){
   printf(lcd_putc, "\fEscritura\nCompletada");
   Delay_ms(DELAY_MESSAGE_MS);
   lcd_putc("\f");
}

void msg_no_program(){
   printf(lcd_putc, "\fNo hay ningun\nprograma cargado");
   Delay_ms(DELAY_MESSAGE_MS);
   lcd_putc("\f");
}

/////////////////////////////Writing Examples//////////////////////////////////
void plc_write_example(){
   msg_loading();
   
   write_ext_eeprom(0x00, 0x10);                                                 //LDN I1
   write_ext_eeprom(0x01, 0x01);                                                 //LD  I2
   write_ext_eeprom(0x02, 0x58);                                                 //O   Q1
   write_ext_eeprom(0x03, 0x40);                                                 //ALD
   write_ext_eeprom(0x04, 0x88);                                                 //=   Q1
   
   write_ext_eeprom(0x05, 0x08);                                                 //LD  Q1
   write_ext_eeprom(0x06, 0x22);                                                 //A   I3
   write_ext_eeprom(0x07, 0x89);                                                 //=   Q2
   
   write_ext_eeprom(0x08, 0xFF);                                                 //End of Code
   write_ext_eeprom(0x09, 0xFF);
   
   msg_writing_succesfull();
}

void plc_write_example_2(){
   msg_loading();
   /*Inversor de Giro
      I1 - OL
      I2 - STOP
      I3 - S1
      I4 - S2
      
      Q1 - M1
      Q2 - M2
      Q3 - L1
      Q4 - L2
   */
   write_ext_eeprom(0x00, 0x10);                                                 //LDN I1
   write_ext_eeprom(0x01, 0x31);                                                 //AN  I2
   write_ext_eeprom(0x02, 0x02);                                                 //LD  I3
   write_ext_eeprom(0x03, 0x58);                                                 //O   Q1
   write_ext_eeprom(0x04, 0x40);                                                 //ALD
   write_ext_eeprom(0x05, 0x39);                                                 //AN  Q2
   write_ext_eeprom(0x06, 0x88);                                                 //=   Q1
   
   write_ext_eeprom(0x07, 0x10);                                                 //LDN I1
   write_ext_eeprom(0x08, 0x31);                                                 //AN  I2
   write_ext_eeprom(0x09, 0x02);                                                 //LD  I3
   write_ext_eeprom(0x0A, 0x58);                                                 //O   Q1
   write_ext_eeprom(0x0B, 0x40);                                                 //ALD
   write_ext_eeprom(0x0C, 0x39);                                                 //AN  Q2
   write_ext_eeprom(0x0D, 0x8A);                                                 //=   Q3
   
   write_ext_eeprom(0x0E, 0x10);                                                 //LDN I1
   write_ext_eeprom(0x0F, 0x31);                                                 //AN  I2
   write_ext_eeprom(0x10, 0x03);                                                 //LD  I4
   write_ext_eeprom(0x11, 0x59);                                                 //O   Q2
   write_ext_eeprom(0x12, 0x40);                                                 //ALD
   write_ext_eeprom(0x13, 0x38);                                                 //AN  Q1
   write_ext_eeprom(0x14, 0x89);                                                 //=   Q2
   
   write_ext_eeprom(0x15, 0x10);                                                 //LDN I1
   write_ext_eeprom(0x16, 0x31);                                                 //AN  I2
   write_ext_eeprom(0x17, 0x03);                                                 //LD  I4
   write_ext_eeprom(0x18, 0x59);                                                 //O   Q2
   write_ext_eeprom(0x19, 0x40);                                                 //ALD
   write_ext_eeprom(0x1A, 0x38);                                                 //AN  Q1
   write_ext_eeprom(0x1B, 0x8B);                                                 //=   Q4
   
   write_ext_eeprom(0x1C, 0xFF);                                                 //End of Code
   write_ext_eeprom(0x1D, 0xFF);
   
   msg_writing_succesfull();
}

//////////////////////////////Running Program//////////////////////////////////
void plc_run_code(){
   msg_loading();
   
   EEPROM_ADDRESS number_of_lines = 0;
   
   int8 loaded_code[512] = {};
   
   while( (loaded_code[number_of_lines] = read_ext_eeprom(number_of_lines) ) != 0xFF)
      number_of_lines++;
   
   if(number_of_lines == 0){
      msg_no_program();
      return;
   }
   
   while(1){
      
      if(input(BACK)){
         output_a(0x00);
         lcd_putc("\f");
         return;
      }
      
      for(EEPROM_ADDRESS i = 0; i < number_of_lines; i++){
         plc_process_instr(loaded_code[i]);
      }
      
      lcd_gotoxy(1, 1);
      printf(lcd_putc, "Q1: %u Q2: %u\nQ3: %u Q4: %u", input(symbols[8]), input(symbols[9]), input(symbols[10]), input(symbols[11]));
   }
   
}

///////////////////////Reviewing List of Instructions//////////////////////////
void plc_show_line(EEPROM_ADDRESS address, int8 command, int8 symbol){
   
   lcd_putc("\f");
   
   #ifndef USE_EXT_EEPROM
      if( (command == CMD_ALD) || (command == CMD_OLD) )
         printf(lcd_putc, "%u - %s", address + 1, commands_text[command]);
      else
         printf(lcd_putc, "%u - %s %s", address + 1, commands_text[command], symbols_text[symbol]);
   #else
      if( (command == CMD_ALD) || (command == CMD_OLD) )
         printf(lcd_putc, "%Lu - %s", address + 1, commands_text[command]);
      else
         printf(lcd_putc, "%Lu - %s %s", address + 1, commands_text[command], symbols_text[symbol]);
   #endif
   
}

void plc_review_code(){
   msg_loading();

   EEPROM_ADDRESS cur_code_line = 0;
   EEPROM_ADDRESS number_of_lines = 0;
   
   int8 loaded_code[512] = {};
   
   while( (loaded_code[number_of_lines] = read_ext_eeprom(number_of_lines) ) != 0xFF)
      number_of_lines++;
      
   if(number_of_lines == 0){  
      msg_no_program();
      return;
   }
   
   plc_show_line(cur_code_line, plc_get_command(loaded_code[cur_code_line]), plc_get_symbol(loaded_code[cur_code_line]));
   
   while(1){
      
      if(input(UP) && (cur_code_line > 0)){
         --cur_code_line;
         plc_show_line(cur_code_line, plc_get_command(loaded_code[cur_code_line]), plc_get_symbol(loaded_code[cur_code_line]));
         Delay_ms(DELAY_BUTTON_PRESS_MS);
      }
      else if(input(DOWN) && (cur_code_line < (number_of_lines-1))){
         ++cur_code_line;
         plc_show_line(cur_code_line, plc_get_command(loaded_code[cur_code_line]), plc_get_symbol(loaded_code[cur_code_line]));
         Delay_ms(DELAY_BUTTON_PRESS_MS);
      }
      
      if(input(BACK))
         return;
   
   }
}

/////////////////////////////USB Connection////////////////////////////////////
void plc_connect_to_pc(){
   EEPROM_ADDRESS cur_code_line = 0;

   int8 in_data[USB_CONFIG_HID_RX_SIZE];
   int8 out_data[USB_CONFIG_HID_TX_SIZE];
   
   int1 rcv_write_flag = 0;
   int1 rcv_read_flag = 0;
   
   printf(lcd_putc, "\fEsperando USB...");
      
   while(TRUE)
   {
      
      if(usb_enumerated()){
         lcd_gotoxy(1, 1);
         printf(lcd_putc, "Conectado       \n");
         if(usb_kbhit(USB_HID_ENDPOINT)){
            
            if(rcv_write_flag == 0 && rcv_read_flag == 0){
               usb_get_packet(USB_HID_ENDPOINT, in_data, USB_CONFIG_HID_RX_SIZE);
               
               if(in_data[0] == 0x33){
                  printf(lcd_putc, "Activado: W");
                  rcv_write_flag = 1;
               }
               
               else if(in_data[0] == 0x22){
                  printf(lcd_putc, "Activado: R");
                  rcv_read_flag = 1;
               }
                  
            }
            
            else if(rcv_write_flag == 1){
               usb_get_packet(USB_HID_ENDPOINT, in_data, USB_CONFIG_HID_RX_SIZE);
               
               write_ext_eeprom(cur_code_line, in_data[0]);
               cur_code_line++;
               
               if(in_data[0] == 0xFF){
                  msg_writing_succesfull();
                  return;
               } 
               
            }
               
         }
         
         if(rcv_read_flag == 1){
         
            EEPROM_ADDRESS number_of_lines = 0;
            
            while(read_ext_eeprom(number_of_lines) != 0xFF)
               number_of_lines++;
               
            for(EEPROM_ADDRESS i = 0; i <= number_of_lines; i++){
               out_data[0] = read_ext_eeprom(i);
               usb_put_packet(USB_HID_ENDPOINT, out_data, USB_CONFIG_HID_TX_SIZE, USB_DTS_TOGGLE);
               Delay_ms(DELAY_SEND_BYTE_TO_PC);
            }
            
            msg_writing_succesfull();
            return;
            
         }
      
      if(input(BACK))
         return;
      
      }
   }
}

/////////////////////////////////Code Erase////////////////////////////////////
void plc_erase(){
   msg_loading();                                                
   
   write_ext_eeprom(0x00, 0xFF);
   write_ext_eeprom(0x01, 0xFF);
   
   msg_writing_succesfull();
}

/////////////////////////////////PLC Menu//////////////////////////////////////
void show_menu_option(int8 option){
   printf(lcd_putc, "\f%s", MENU_OPTIONS[option]);
   Delay_ms(DELAY_BUTTON_PRESS_MS);
}

void enter_menu_option(int8 option){

   switch(option){
      case 0:  plc_run_code();         break;
      case 1:  plc_review_code();      break;
      case 2:  plc_connect_to_pc();    break;
      case 3:  plc_write_example();    break;
      case 4:  plc_write_example_2();  break;
      case 5:  plc_erase();            break;
   }
   
}
