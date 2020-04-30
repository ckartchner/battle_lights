// A basic everyday NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 60

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


// setup() function -- runs once at startup --------------------------------

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  Serial.begin(9600);
}


// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {
  // // Fill along the length of the strip in various colors...
  // colorWipe(strip.Color(255,   0,   0), 50); // Red
  // colorWipe(strip.Color(  0, 255,   0), 50); // Green
  // colorWipe(strip.Color(  0,   0, 255), 50); // Blue

  // // Do a theater marquee effect in various colors...
  // theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
  // theaterChase(strip.Color(127,   0,   0), 50); // Red, half brightness
  // theaterChase(strip.Color(  0,   0, 127), 50); // Blue, half brightness

  //  rainbow(10);             // Flowing rainbow cycle along the whole strip
//  theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
  rvb();
}

int rand_range = 3;
int new_pos(int curr_pos) {
  curr_pos = curr_pos + random(-rand_range, rand_range);
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

int middle = strip.numPixels()/2;
int quarter = middle / 2;
int blue_team_init_pos = strip.numPixels() - quarter;
int red_team_init_pos = quarter;
int blue_team_pos = blue_team_init_pos;
int red_team_pos = red_team_init_pos;
uint32_t red = strip.Color(127, 0, 0);
uint32_t blue = strip.Color(0, 0, 127);
uint32_t yellow = strip.Color(127, 127, 0);
uint32_t green = strip.Color(0, 127, 0);
uint32_t purple = strip.Color(127, 0, 127);

void rvb() {
  strip.clear();
  for(int i=0; i<middle; i++) {
    strip.setPixelColor(i, red);
  }
  for(int i=middle; i<=strip.numPixels(); i++) {
    strip.setPixelColor(i, blue);
  }
  int blue_team_start_pos = blue_team_pos;
  int red_team_start_pos = red_team_pos;
  int blue_team_new_pos = new_pos(blue_team_pos);
  int red_team_new_pos = new_pos(red_team_pos);
  float col_time = collision_time(blue_team_start_pos, blue_team_new_pos, red_team_start_pos, red_team_new_pos);
  Serial.print("collision time:");
  Serial.println(col_time);
  if (is_collision(col_time) == true) {
    float cp = collision_position(blue_team_start_pos, blue_team_new_pos, col_time);
    // check which team scored the point
    // increment the score
    // do cool flashy light stuff here to indicate end/start of round
    Serial.print("Blue start:");
    Serial.println(blue_team_start_pos);
    Serial.print("Blue stop:");
    Serial.println(blue_team_new_pos);
    Serial.print("Red start:");
    Serial.println(red_team_start_pos);
    Serial.print("Red stop:");
    Serial.println(red_team_new_pos);
    Serial.print("Collision position:");
    Serial.println(cp);
    Serial.println("Show a pretty rainbow");
    rainbow(2);
    blue_team_new_pos = blue_team_init_pos;
    red_team_new_pos = red_team_init_pos;
  }
  blue_team_pos = normalize_pos(blue_team_new_pos);
  red_team_pos = normalize_pos(red_team_new_pos);
  strip.setPixelColor(blue_team_pos, purple);
  strip.setPixelColor(red_team_pos, yellow);
  strip.show();
  delay(100);
}

// boolean sign_changed(int num1, int num2){
//   boolean output = false;
//   if (num1 > 0 && num2 < 0) {
//     output = true;
//   } else if (num1 < 0 && num2 > 0) {
//     output = true;
//   }
//   return output;
// }

float collision_time(int pos_a1, int pos_a2, int pos_b1, int pos_b2) {
  // Derived from 
  Serial.print(pos_a1);
  Serial.print(pos_a2);
  Serial.print(pos_b1);
  Serial.print(pos_b2);
  float col_time = float(pos_b1 - pos_a1)/(pos_a2 - pos_a1 - pos_b2 + pos_b1);
  return col_time;
}

boolean is_collision(float time) {
  boolean collision = false;
  if (0 < time && time <= 1) {
    Serial.print("Collision set with time:");
    Serial.println(time);
    collision = true;
  }
  return collision;
}

float collision_position(int pos1, int pos2, float time) {
  int position;  // no collision occurs
  if (0 <= collision_time <= 1) {
    position = pos1 + (pos2-pos1) * time;
  }
  return position;
}

// int crossing_pixel() {
//   int prev_dist = blue_origin - red_origin;
//   int new_dist = new_blue - new_red; 
//   boolean crossing = false;
//   if sign_changed(prev_dist, new_dist) {
//     crossing = true;
//   }

//   return crossing;
// } 

// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
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

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
