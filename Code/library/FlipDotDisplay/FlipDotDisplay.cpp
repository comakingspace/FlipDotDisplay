/*

*/

#include "Arduino.h"
#include "FlipDotDisplay.h"

void FlipDotDisplay::init(int16_t sizeX, int16_t sizeY){
    _sizeX = sizeX;
    _sizeY = sizeY;
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
  int x_text = x;
  int y_text = y;

  /*Serial.print("X:");
  Serial.print(x_text,DEC);
  Serial.print(" Y:");
  Serial.println(y_text,DEC);*/

  if(_rotation == 0){
    if(x < _sizeX || y < _sizeY){
      bitWrite(drawBuffer[ ((_sizeX/8) * y) + (x/8) ],
               7-(x%8),
               state);
    }
  }
  else{
    bitWrite(drawBuffer[(x*2) + (1-(y/8))],
             (7-(y%8)),
             state);
  }
}

void FlipDotDisplay::writeData(uint8_t data[]){
    uint8_t serialPacket[_serialSize];

    /*
    Martins code...

    int exactTargetBit = 0;
    if ( _sizeX > _sizeY) {
      for (int i = 0; i<144; i++){
        for (int j = 0; j < 8; j++){
          int exactBit = ((i*j)+j);
          if ((exactBit % 2) == 0){
            exactTargetBit = exactBit / 2;
          }
          else{
            exactTargetBit = exactBit / 2 + 576;
          }
          bitWrite(r_data[(exactTargetBit/8)],exactTargetBit % 8,bitRead(data[i],j));
          //targetX = exactBit % 47;
          //targetY = exactBit % 23;
          //exactTargetBit = targetX;
          //bitWrite(r_data[i],j);
        }
      }
    }
    */
    //replacemnt array
    uint8_t r_data[144];
    // check wheather FDD is in landscape
    if ( _sizeX > _sizeY){
      //bool arrays to simplify rotation
      bool b_data[24][48];
      bool b_r_data[24][48];
      //replacemnt array
      uint8_t r_data[144];

      //convert byte array to bool
      for(int i = 0; i<144; i++){
        for(int j = 0; j<8; j++){
          b_data[i % 3 * 8 + j][i/3]
          = (bool)bitRead(data[i],7-j);
        }
      }

      //algorithm for rotation
      for(int i = 0; i<24; i++){
        for(int j = 0; j<48; j++){
          b_r_data[23-i][j] = b_data[j%24][j/24 + i*2];
        }
      }

      //convert back to byte array
      for(int i = 0; i<144; i++){
        for(int j = 0; j<8; j++){
          bitWrite(r_data[i],
                   7-j,
                   b_r_data[i % 3 * 8 + j][i/3]);
        }
      }
      //copy rotated array
      memcpy(data, r_data, sizeof(byte)*144);
    }


    serialPacket[0] = _header;

    for(int i = 0; i < (_serialSize-2); i++){
      serialPacket[i+1] = data[i];
    }
    serialPacket[_serialSize-1] = checksum(serialPacket);

    digitalWrite(LED_BUILTIN, LOW);
    Serial.write(serialPacket, _serialSize);

    digitalWrite(LED_BUILTIN, HIGH);
}

void FlipDotDisplay::display(void){
    writeData(drawBuffer);
}

void FlipDotDisplay::clear(void){
    for (int i = 0; i < _serialSize-2; i++){
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
