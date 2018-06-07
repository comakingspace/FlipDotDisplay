/*
 * This scetch executes the basic text display 
 * functions of the FLipDotDisplay library.
 * - write normal Strings
 * - display a single char
 * - simple scolling text
 * 
 * by Patrick Kübler
 * License: Creative Commons 4.0
 */

#include <Arduino.h>
#include <FlipDotDisplay.h>
#include <Adafruit_GFX.h>

// initialize display with 24x16 dots
FlipDotDisplay flip(24,16);

void setup() {
    flip.setTextWrap(true);
}

void loop() {
    //erase draw buffer
    flip.clear();
    //Fill in draw buffer
    flip.print("Hello World");
    //Push to display
    flip.display();

    delay(3000);

    flip.clear();
    flip.print("What's up?");
    flip.display();

    delay(3000);

    //display A-Z
    for (uint8_t i = 0x41; i < 0x5B; i++) {
      flip.clear();
      flip.drawChar(8,4,i,1,0,1);
      flip.display();
      delay(200);
    }

    //scroll text
    flip.setTextWrap(false);
    String text = "This is a very long string so it won't display in one line...";

    for (int pos =  -1 * text.length(); pos <= 0; pos++) {
      flip.clear();
      flip.setCursor(6 * pos, 4);
      flip.print(text);
      flip.display();
      delay(500);
    }

    delay(1000);
}
