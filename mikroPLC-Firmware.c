//#define USE_EXT_EEPROM
//#define USE_2404

//#define USE_LCD_I2C

#include <mikroPLC.c>

void main()
{
   set_tris_a (0x00) ;
   set_tris_d (0xFF) ;
   
   output_a (0x00) ;
   
   disable_interrupts (GLOBAL) ;
   
   lcd_init () ;
   
   usb_init () ;
   
   init_ext_eeprom () ;
   
   int8 plc_menu_option = 0;
   
   show_menu_option (plc_menu_option) ;
   
   while (1)
   {
      
      if (input (UP) && (plc_menu_option > 0) )
      {
         plc_menu_option--;
         show_menu_option (plc_menu_option) ;
      }

      else if (input (DOWN) && (plc_menu_option < (NUMBER_OF_MENU_OPTIONS - 1 )))
      {
         plc_menu_option++;
         show_menu_option (plc_menu_option) ;
      }

      else if (input (ENTER))
      {
         enter_menu_option (plc_menu_option) ;
         show_menu_option (plc_menu_option) ;
      }
   }
}

