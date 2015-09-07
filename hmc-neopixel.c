#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

 #define RAINBOW 0
 #define THEATER_CHASE 1
 #define THEATER_CHASE_RAINBOW 2
 #define COLOR 3
 #define COLOR_WIPE 4
 
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
int activeProgram = 0;
boolean active = false;
byte red, green, blue;

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

// initialize serial:
  Serial.begin(9600);

  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  red = green = blue = 0;

  Serial.println("Welcome to NEOPIXEL! Please tell me what to do!!");
  Serial.println(activeProgram);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void parseColors(String colorString) {
  red   = (byte) getValue(colorString, ' ', 1).toInt();
  green = (byte) getValue(colorString, ' ', 2).toInt();
  blue   = (byte) getValue(colorString, ' ', 3).toInt();
//  Serial.println("Parsed the following colors: ");
//  Serial.println(red);
//  Serial.println(green);
//  Serial.println(blue);
}

void parseCommands() {
   if (stringComplete) {
    //Serial.println("Recieved: " + inputString); 

    if (inputString == "on")  { active = true; }
    if (inputString == "off") { active = false; }

    String command = getValue(inputString, ' ', 0);

    if (command == "status") {
      String fuck = (active?"true":"false");
      String response = String("active " + fuck);
      Serial.println(response);
      
    } else if (command == "rainbow") {
      activeProgram = RAINBOW;
      
    } else if (command == "theaterChase") {
      activeProgram = THEATER_CHASE;
      parseColors(inputString);
      
    } else if (command == "theaterChaseRainbow") {
      activeProgram = THEATER_CHASE_RAINBOW;
      
    } else if (command == "color") {
      activeProgram = COLOR;
      parseColors(inputString);
      
    } else if (command == "colorWipe") {
      activeProgram = COLOR_WIPE;
      parseColors(inputString);
      
    } else {
      Serial.println("Invalid command");
    } 
    
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

void loop() {
  parseCommands();
  
  if (active) {
     switch (activeProgram) {
      case RAINBOW: rainbow(20);  break;
      case THEATER_CHASE: theaterChase(strip.Color(red, green, blue), 50);  break;
      case THEATER_CHASE_RAINBOW: theaterChaseRainbow(50); break;
      case COLOR: setColor(strip.Color(red, green, blue));  break; 
      case COLOR_WIPE: colorWipe(strip.Color(red, green, blue), 50);  break;
     }
  } else {
    clear();
  }
  
}

// Split a string separated with separator characters, 
// and get the value for specified index (zero based)
// Example: getValue("hello 123 45", ' ',1) -> "123" 
String getValue(String data, char separator, int index) {
    int maxIndex = data.length()-1;
    int j=0;
    String chunkVal = "";
    for (int i=0; i <= maxIndex && j <= index; i++) {
      chunkVal.concat(data[i]);
      if (data[i] == separator) {
        j++;
        if (j > index) {
          chunkVal.trim();
          return chunkVal;
        }    
        chunkVal = "";    
      }  
    }  
}

// Fill entire strip with a specific color
void setColor(uint32_t color) {
  uint16_t i;
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
  delay(10);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Make strip black color
void clear() {
  uint16_t i;
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
  delay(10);
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      if (!active) {
        return;
      }
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char) Serial.read(); 
    if (inChar != '\n') {
      inputString += inChar;
    }
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}


