#define EEPROM_ADDRESS        BYTE
#define EEPROM_SIZE           256

void init_ext_eeprom() {
   
}

BOOLEAN ext_eeprom_ready() {
   return 1;
}

void write_ext_eeprom(BYTE address, BYTE data) {
   write_eeprom(address, data);
   Delay_ms(DELAY_MEMORY_WRITE_MS);
}

BYTE read_ext_eeprom(BYTE address) {
   BYTE data;
   
   data=read_eeprom(address);
   Delay_ms(DELAY_MEMORY_READ_MS);

   return(data);
}
