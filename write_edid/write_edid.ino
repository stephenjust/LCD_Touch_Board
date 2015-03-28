/* This code is from Adafruit's guide for their LCD module
 * https://learn.adafruit.com/adafruit-tfp401-hdmi-slash-dvi-decoder-to-40-pin-ttl-display/editing-the-edid
 */
#include <Wire.h> //I2C library

/* 800x480 ! */ 
const uint8_t PROGMEM eepromdat[128] = {
0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x04, 0x81, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00,
0x01, 0x11, 0x01, 0x03, 0x80, 0x0F, 0x0A, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x80, 0x0C, 0x20, 0x80, 0x30, 0xE0, 0x2D, 0x10, 0x28, 0x30,
0xD3, 0x00, 0x6C, 0x44, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17,
};

// I2C eeprom dumper
byte i2c_eeprom_read_byte(uint8_t deviceaddress, uint16_t eeaddress );

#define EEPROMSIZE 256UL  // 0.5 Kb
#define ADDRESS_SIZE 8

byte i2c_eeprom_read_byte(uint8_t deviceaddress, uint16_t eeaddress ) {
    byte rdata = 0xFF;
#if (ADDRESS_SIZE == 16)
    Wire.beginTransmission(deviceaddress);
    Wire.write((eeaddress >> 8)); // MSB
#else
    //deviceaddress |= (eeaddress >> 8);
    Wire.beginTransmission(deviceaddress); // MSB
#endif
    Wire.write(eeaddress); // LSB
    Wire.endTransmission();
    Wire.requestFrom(deviceaddress, (uint8_t)1);
    while (!Wire.available()); 
    rdata = Wire.read();
    return rdata;
}

void i2c_eeprom_write_byte(uint8_t deviceaddress, uint16_t eeaddress, byte data ) {
#if (ADDRESS_SIZE == 16)
    Wire.beginTransmission(deviceaddress);
    Wire.write((eeaddress >> 8)); // MSB
#else
    //deviceaddress |= (eeaddress >> 8);
    Wire.beginTransmission(deviceaddress); // MSB
#endif
    Wire.write((byte)eeaddress); // LSB
    Wire.write((byte)data);
    Wire.endTransmission();
}
  

void setup() {
  Wire.begin(); // initialise the connection
  Serial.begin(9600);
  Serial.println(F("EEPROM WRITER"));
  Serial.print(F("EEPROM data size: "));
  Serial.println(sizeof(eepromdat));
  Serial.println(F("Hit any key & return to start"));
  while (!Serial.available());
  byte b;
  Serial.println("Starting");
  for (uint16_t addr = 0; addr < EEPROMSIZE; addr++) {
    if (addr < sizeof(eepromdat)) {
      b = pgm_read_byte(eepromdat+addr);
    } else {
      b = 0xFF;
    }
     i2c_eeprom_write_byte(0x50, addr, b);
     delay(5);
     if ((addr % 32) == 0)
       Serial.println();
     Serial.print("0x");
     if (b < 0x10) Serial.print('0');
     Serial.print(b, HEX); //print content to serial port
     Serial.print(", ");
  }
  Serial.println("\n\r========\n\rFinished!");
  for (uint16_t addr = 0; addr < EEPROMSIZE; addr++) {
    if (addr < sizeof(eepromdat)) {
      b = pgm_read_byte(eepromdat+addr);
    } else {
      b = 0xFF;
    }
    uint8_t d =  i2c_eeprom_read_byte(0x50, addr);
    if ((addr % 32) == 0)
      Serial.println();
    Serial.print("0x");
    if (d < 0x10) Serial.print('0');
    Serial.print(d, HEX); //print content to serial port
    Serial.print(", ");

    if (b != d) {
       Serial.print(F("verification failed at 0x")); Serial.println(addr);
       while (1);
     }
  }
  Serial.println(F("\n\r\n\rVerified!"));
}

void loop() {
}

