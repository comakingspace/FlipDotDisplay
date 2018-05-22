/*

*/

#include "Arduino.h"
#include "FlipDotDisplay.h"

void FlipDotDisplay::init(int16_t sizeX, int16_t sizeY){
    //_sizeX = sizeX;
    //_sizeY = sizeY;
    // Initialize serial package size header
    // header + payload + checksum
    _serialSize = (sizeX * sizeY)/8 +2;
    //_serialSize = 2 + ((_sizeX * sizeY)/8);
    uint8_t drawBuffer[_serialSize-2];
    uint8_t serialBuffer[_serialSize];

    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
}

FlipDotDisplay::FlipDotDisplay(int16_t sizeX, int16_t sizeY) : Adafruit_GFX(sizeX, sizeY) {
        init(sizeX, sizeY);
    }


void FlipDotDisplay::drawPixel(int16_t x, int16_t y, uint16_t state){
  if (state == 0) {
    bool colour = 0;
  }
  else{
    bool colour = 1;
  }
  bitWrite(drawBuffer[ (3*y) + (x/8) ],
           7-(x%8),
           state);
}

void FlipDotDisplay::writeData(uint8_t data[]){
    uint8_t serialPacket[_serialSize];
    serialPacket[0] = _header;

    for(int i = 0; i < (_serialSize-2); i++){
      serialPacket[i+1] = data[i];
    }
    serialPacket[_serialSize-1] = checksum(serialPacket);

    digitalWrite(LED_BUILTIN, LOW);
    Serial.write(serialPacket, _serialSize);
    digitalWrite(LED_BUILTIN, HIGH);
}

void FlipDotDisplay::display(void) {
    writeData(drawBuffer);
}

void FlipDotDisplay::clear(void) {
    for (int i = 0; i < _serialSize-2; i++) {
      drawBuffer[i] = 0b00000000;
    }
}

uint8_t FlipDotDisplay::checksum(uint8_t data[]){
    //return 0xD0;
    uint8_t result = 0;
    uint8_t sum = 0;

    for (int i = 0; i < (_serialSize-1); i++){
      sum += data[i];
    }

    result = sum & 0xFF;

    return result;
}
