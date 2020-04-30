/*************************************************** 
  This is a library for our I2C LED Backpacks

  Designed specifically to work with the Adafruit LED 7-Segment backpacks 
  ----> http://www.adafruit.com/products/881
  ----> http://www.adafruit.com/products/880
  ----> http://www.adafruit.com/products/879
  ----> http://www.adafruit.com/products/878

  These displays use I2C to communicate, 2 pins are required to 
  interface. There are multiple selectable I2C addresses. For backpacks
  with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
  with 3 Address Select pins: 0x70 thru 0x77

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment matrix = Adafruit_7segment();
int left_count = 0;
int right_count = 0;
int full_count = 0;

void setup() {
#ifndef __AVR_ATtiny85__
  Serial.begin(9600);
  Serial.println("7 Segment Backpack Test");
#endif
  matrix.begin(0x70);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
}

void loop() {
  int rightButton = digitalRead(2);
  int leftButton = digitalRead(3);
  if (leftButton == LOW) {
    left_count++;
  }
  if (rightButton == LOW) {
    right_count++;
  }

  full_count = left_count * 100 + right_count;
  matrix.print(full_count, DEC);
  // Ensure there is always a zero in the left most digit
  if (full_count < 1000) {
    matrix.writeDigitNum(0, 0);
  }
  // Initialize display to all zeros
  if (left_count < 1){
      matrix.writeDigitNum(0, 0);
      matrix.writeDigitNum(1, 0);
      matrix.writeDigitNum(2, 0);
      matrix.writeDigitNum(3, 0);
      matrix.drawColon(false);
  }

  matrix.writeDisplay();
  delay(100);
}
