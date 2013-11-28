#include <OneWire.h>

// DS18S20 Temperature chip i/o
OneWire ds(24);  // on pin 10

void setup(void) {
  // initialize inputs/outputs
  // start serial port
  Serial.begin(9600);
}

void loop(void) {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      ds.reset_search();
      return;
  }

  for( i = 0; i < 8; i++) {
    //Serial.print(addr[i], HEX);
    //Serial.print(" ");
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.print("CRC is not valid!\n");
      return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end

  //delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad

  //Serial.print(present,HEX);
  for (i=0; i<9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  long int temperature;
  temperature = data[0] >> 4;
  temperature += (data[1] & 0b00000111) << 4;
  temperature *= 10000;
  int fraction = data[0] & 0b00001111;
  for (int i=0; i<4; i++) {
      if (fraction & 1 << i) {
          switch (i) {
              case 0:
                  temperature += 625;
                  break;
              case 1:
                  temperature += 1250;
                  break;
              case 2:
                  temperature += 2500;
                  break;
              case 3:
                  temperature += 5000;
                  break;
          }
      }
  }
  //Serial.print( OneWire::crc8( data, 8), HEX);
  Serial.println(temperature);
}
