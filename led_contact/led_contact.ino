// Bootstrapped with NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 150

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

// number of pixesl the led can randomly move
int rand_range = 3;

Adafruit_7segment matrix = Adafruit_7segment();
int left_count = 0;
int right_count = 0;
int full_count = 0;
uint8_t previous_winner = 0x00;

int middle = strip.numPixels()/2;
int quarter = middle / 2;
int right_team_init_pos = strip.numPixels() - quarter;
int left_team_init_pos = quarter;
int right_team_pos = right_team_init_pos;
int left_team_pos = left_team_init_pos;
uint32_t red = strip.Color(127, 0, 0);
uint32_t blue = strip.Color(0, 0, 127);
uint32_t yellow = strip.Color(127, 127, 0);
uint32_t green = strip.Color(0, 127, 0);
uint32_t purple = strip.Color(127, 0, 127);
uint32_t left_color = red;
uint32_t right_color = blue;



void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  // LED setup
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  white_flash();
  strip.show();
  
  // 7 segment setup
  matrix.begin(0x70);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  zero_seven_segment();
  delay(5000);
  Serial.begin(9600);
}


void loop() {
  pixel_battle();
}


int new_pos(int curr_pos) {
  curr_pos = curr_pos + random(-rand_range, rand_range + 1);
  return curr_pos;
}


int normalize_pos(int curr_pos) {
  if (curr_pos >= LED_COUNT) {
    curr_pos = curr_pos - LED_COUNT;
  } else if (curr_pos < 0) {
    curr_pos = curr_pos + LED_COUNT;
  }
  return curr_pos;
}


void zero_seven_segment() {
  matrix.writeDigitRaw(2, previous_winner);
  matrix.writeDigitNum(0, 0);
  matrix.writeDigitNum(1, 0);
  matrix.writeDigitNum(3, 0);
  matrix.writeDigitNum(4, 0);
  matrix.writeDisplay();
}


void reset_all_scores() {
  left_count = 0;
  right_count = 0;
  full_count = 0;
  zero_seven_segment();
}


void over99(String side){
  /*
  Check if one side has more than 99 points
  The seven segment display is limited to a maximum of 99 points per side.

  if yes:
    zero the scores
    add a dot indicating previous winning
    run total victory sequence
  
  */

  if (side == "left") {
    // Set left colon lower dot -- slightly more left
    previous_winner = 0x08;
  } else if (side == "right") {
    // set left colon upper dot -- slightly more right
    previous_winner = 0x04;
  }
  reset_all_scores();
  rainbow(10);
}


void show_battle_pos(int right_team_new_pos, int left_team_new_pos){
  right_team_pos = normalize_pos(right_team_new_pos);
  left_team_pos = normalize_pos(left_team_new_pos);
  strip.setPixelColor(right_team_pos, purple);
  strip.setPixelColor(left_team_pos, green);
  strip.show();
}


void pixel_battle() {
  strip.clear();
  // Setup left vs. right colors
  for(int i=0; i<middle; i++) {
    strip.setPixelColor(i, left_color);
  }
  for(int i=middle; i<=strip.numPixels(); i++) {
    strip.setPixelColor(i, right_color);
  }
  int right_team_start_pos = right_team_pos;
  int left_team_start_pos = left_team_pos;
  int right_team_new_pos = new_pos(right_team_pos);
  int left_team_new_pos = new_pos(left_team_pos);
  show_battle_pos(right_team_new_pos, left_team_new_pos);
  float col_time = collision_time(right_team_start_pos, right_team_new_pos, left_team_start_pos, left_team_new_pos);
  if (is_collision(col_time) == true) {
    float cp = collision_position(right_team_start_pos, right_team_new_pos, col_time);
    // Check which team scored the point and increment 
    if (cp < middle) {
      right_count++;
    } else if (cp > middle) {
      left_count++;
    } else {
      // Tie
    }
    if (left_count > 99) {
      over99("left");
    } else if (right_count > 99) {
      over99("right");
    } else {
      full_count = left_count * 100 + right_count;
      Serial.println(full_count);
      matrix.print(full_count, DEC);
      // Ensure there is always a zero in the left most digit
      if (full_count < 1000) {
          matrix.writeDigitNum(0, 0);
      }
      if (full_count < 100) {
        matrix.writeDigitNum(1, 0);
      }
      if (full_count < 10) {
        matrix.writeDigitNum(3, 0);
      }
      matrix.writeDigitRaw(2, previous_winner);
      matrix.writeDisplay();

      // End of round LED sequence
      if (cp < middle) {
        right_wins();
      } else if (cp > middle) {
        left_wins();  
      } else {
        // Tie
      }
      white_flash();
    }
    // Reset pixels to starting location
    show_battle_pos(right_team_init_pos, left_team_init_pos);
  }
  delay(100);
}

float collision_time(int pos_a1, int pos_a2, int pos_b1, int pos_b2) {
  // At each step, the "velocity" of each point changes
  // The velocity is constant during that step
  // time = 
  // (how far apart the points were when they started) /
  // (difference in velocity for the two points) 
  float col_time = float(pos_b1 - pos_a1)/(pos_a2 - pos_a1 - pos_b2 + pos_b1);
  return col_time;
}


boolean is_collision(float time) {
  // determine if collision occured
  boolean collision = false;
  if (0 < time && time <= 1) {
    collision = true;
  }
  return collision;
}


float collision_position(int pos1, int pos2, float time) {
  // determine where a collision might occur  
  int position;  // no collision occurs
  if (0 <= collision_time <= 1) {
    position = pos1 + (pos2-pos1) * time;
  }
  return position;
}


// Some functions for creating animated effects -----------------
void right_wins() {
  for (int i=middle; i>=0; i--) {
    strip.setPixelColor(i, right_color);
    strip.show();
    delay(200);
  }
}


void left_wins() {
  for (int i=middle; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, left_color);
    strip.show();
    delay(200);
  }
}


void white_flash() {
  for (int i=0; i<3; i++) {
    strip.fill(strip.Color(127, 127, 127));  // White half brightness
    strip.show();
    delay(200);
    strip.clear();
    strip.show();
    delay(100);
  }
}
// // Fill strip pixels one after another with a color. Strip is NOT cleared
// // first; anything there will be covered pixel by pixel. Pass in color
// // (as a single 'packed' 32-bit value, which you can get by calling
// // strip.Color(red, green, blue) as shown in the loop() function above),
// // and a delay time (in milliseconds) between pixels.
// void colorWipe(uint32_t color, int wait) {
//   for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
//     strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
//     strip.show();                          //  Update strip to match
//     delay(wait);                           //  Pause for a moment
//   }
// }

// // Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// // a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// // between frames.
// void theaterChase(uint32_t color, int wait) {
//   for(int a=0; a<10; a++) {  // Repeat 10 times...
//     for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
//       strip.clear();         //   Set all pixels in RAM to 0 (off)
//       // 'c' counts up from 'b' to end of strip in steps of 3...
//       for(int c=b; c<strip.numPixels(); c += 3) {
//         strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
//       }
//       strip.show(); // Update strip with new contents
//       delay(wait);  // Pause for a moment
//     }
//   }
// }

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 15 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 15*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// // Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
// void theaterChaseRainbow(int wait) {
//   int firstPixelHue = 0;     // First pixel starts at red (hue 0)
//   for(int a=0; a<30; a++) {  // Repeat 30 times...
//     for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
//       strip.clear();         //   Set all pixels in RAM to 0 (off)
//       // 'c' counts up from 'b' to end of strip in increments of 3...
//       for(int c=b; c<strip.numPixels(); c += 3) {
//         // hue of pixel 'c' is offset by an amount to make one full
//         // revolution of the color wheel (range 65536) along the length
//         // of the strip (strip.numPixels() steps):
//         int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
//         uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
//         strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
//       }
//       strip.show();                // Update strip with new contents
//       delay(wait);                 // Pause for a moment
//       firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
//     }
//   }
// }
