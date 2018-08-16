/*
  FlipDotDisplay.h for controlling FlipDotDisplays
*/

#ifndef FlipDotDisplay_h
#define FlipDotDisplay_h


#include "Arduino.h"
#include "Adafruit_GFX.h"

class FlipDotDisplay : public Adafruit_GFX {

	public:

    //Constructor
	  FlipDotDisplay(int16_t sizeX, int16_t sizeY);
		uint8_t drawBuffer[255];
		uint8_t serialBuffer[255];

	 void
	    writeData(uint8_t data[]),
	    drawPixel(int16_t x, int16_t y, uint16_t state),
			display(void),
	    clear(void);
			//setRotation(int16_t);


	private:

	  uint8_t checksum(uint8_t data[]);
	  uint8_t _sizeX;
	  uint8_t _sizeY;
	  uint8_t _header = 0xaa;
	  uint8_t _serialSize;
		uint8_t _rotation = 0;

    void init(int16_t sizeX, int16_t sizeY);
};

#endif
