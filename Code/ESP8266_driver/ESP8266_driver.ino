
/*
 * FlipDotDisplay ESP8266 Firmware
 *
 * This is the software to generate display data and send it via serial stream
 * to the Atmel328 for display. The ESP acts also as the interface to send data
 * to the display. Right now only UDP for interfacing is implemented.
 *
 * ToDo:
 * Adafruit_GFX implementation
 * Read tweets and display them
 *
 * by Patrick Kübler (2018)
 * License: Creative Commons 4.0
 */


#include <Adafruit_GFX.h>
#include <FlipDotDisplay.h>
#include <NTPClient.h>

FlipDotDisplay flip(24,16);


#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


const char* ssid = "freifunk-rhein-neckar.de";
const char* password = NULL;

const char* host = "www.twitter.com/pakuee";

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
uint8_t incomingPacket[255];  // buffer for incoming packets
uint8_t  replyPacket[] = "Hi there! Got the message :-)";  // a reply string to send back
uint8_t drawBuffer[48];

const int serialSize = 50;
//byte dataBuffer[bufferSize];
byte header = 0xAA;
//byte dataBuffer[serialSize];

NTPClient timeClient(Udp);

byte boot[48] = {
  B11100111,B10111101,B11000000,
  B10010100,B10100100,B10000000,
  B11100100,B10100100,B10000000,
  B10010100,B10100100,B10000000,
  B11100111,B10111100,B10101010,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B11111111,B11111111,B11111111,
  B10000000,B00000000,B00000001,
  B11111111,B11111111,B11111111,
  B00000000,B00000000,B00000000
};

byte Meh[48] = {
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B11100000,B01010010,B00000000,
  B00100000,B10100101,B00000111,
  B00010000,B10001101,B00001000,
  B00001000,B10011001,B00010000,
  B00000111,B01000010,B11100000,
  B00000000,B00000000,B00000000
};

byte all[48] = {
  B11111111,B11111111,B11111111,
  B11111111,B11111111,B11111111,
  B11111111,B11111111,B11111111,
  B11111111,B11111111,B11111111,
  B11111111,B11111111,B11111111,
  B11111111,B11111111,B11111111,
  B11111111,B11111111,B11111111,
  B11111111,B11111111,B11111111,
  B11111111,B11111111,B11111111,
  B11111111,B11111111,B11111111,
  B11111111,B11111111,B11111111,
  B11111111,B11111111,B11111111,
  B11111111,B11111111,B11111111,
  B11111111,B11111111,B11111111,
  B11111111,B11111111,B11111111,
  B11111111,B11111111,B11111111
};

byte connecting[48] = {
  B10001000,B00100000,B00000000,
  B10101010,B01001000,B00000000,
  B11111000,B11100000,B00000000,
  B11011010,B01001000,B00000000,
  B10001010,B01001010,B10100000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000
};

byte connecting2[48] = {
  B10001000,B00100000,B00000000,
  B10101010,B01001000,B00000000,
  B11111000,B11100000,B00000000,
  B11011010,B01001000,B00000000,
  B10001010,B01001010,B10000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000
};

byte wifiOK[48] = {
  B10001000,B00100000,B00000000,
  B10101010,B01001000,B00001000,
  B11111000,B11100000,B00010000,
  B11011010,B01001000,B10100000,
  B10001010,B01001000,B01000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000
};

byte Face[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x0f,0x0f,0x80,0x09,0x08,0x80,0x13,0x1c,0x80,0x13,0x1f,0xb0,0x1e,0x00,0x60,0x00,0x00,0xc0,0x00,0x07,0x80,0x07,0x8f,0x80,0x08,0x7f,0x80,0x00,0x07,0x80,0x00,0x03,0x80,0x00,0x00,0x00};

byte pacman1[]={0x00,0x7f,0x00,0x01,0xff,0x80,0x01,0xff,0xc0,0x03,0xdf,0xc0,0x07,0xff,0xc0,0x07,0xff,0x00,0x07,0xfc,0x00,0x07,0xf8,0x00,0x07,0xfe,0x00,0x07,0xff,0x80,0x07,0xff,0xc0,0x07,0xff,0x80,0x01,0xff,0x80,0x00,0x3e,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
byte pacman2[]={0x00,0x7f,0x00,0x01,0xff,0x80,0x01,0xff,0xc0,0x03,0xdf,0xc0,0x07,0xff,0xc0,0x07,0xff,0xc0,0x07,0xff,0xc0,0x07,0xff,0xc0,0x07,0xff,0xc0,0x07,0xff,0xc0,0x07,0xff,0xc0,0x07,0xff,0x80,0x01,0xff,0x80,0x00,0x3e,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

byte bottleGlass[]={0x03,0x00,0x00,0x03,0x00,0x00,0x03,0x00,0x00,0x03,0x00,0x00,0x07,0x84,0x10,0x07,0x84,0x10,0x0f,0xc6,0x10,0x0f,0xc2,0x30,0x08,0x43,0xe0,0x08,0x40,0x80,0x0f,0xc0,0x80,0x0f,0xc0,0x80,0x0f,0xc0,0x80,0x0f,0xc0,0x80,0x0f,0xc0,0x80,0x0f,0xc3,0xe0};

byte eyes1[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x07,0xc0,0x23,0x04,0x60,0x41,0x0d,0x20,0x49,0x08,0x20,0x63,0x0e,0x60,0x3e,0x03,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
byte eyes2[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x07,0xc0,0x23,0x05,0x60,0x41,0x0c,0x20,0x41,0x08,0x20,0x6b,0x0e,0x60,0x3e,0x03,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

String timeString = "";

void setup() {
  flip.writeData(connecting);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    flip.writeData(connecting2);
    delay(300);
    flip.writeData(connecting);
  }

  flip.writeData(wifiOK);

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  timeClient.begin();

  flip.setTextWrap(false);
}

void loop() {

  timeClient.update();
  timeString = timeClient.getFormattedTime();
  
  flip.clear();
  
  flip.drawChar(0,2,
    timeString.charAt(0),
    1,0,1);
  flip.drawChar(6,2,
    timeString.charAt(1),
    1,0,1);
  flip.drawChar(13,6,
    timeString.charAt(3),
    1,0,1);
  flip.drawChar(19,6,
    timeString.charAt(4),
    1,0,1);
  flip.display();

  delay(1000);
  
}

/*

void updateDisplay(){
  writeData(drawBuffer);
  for(int i = 0; i<48; i++){
    drawBuffer[i] = B00000000;
  }
}

/*
void drawPixel(uint8_t x, uint8_t y, uint8_t colour){
  bitWrite(drawBuffer[ (3*y) + (x/8) ],
           7-(x%8),
           colour);
}
*/

byte checksum(byte data[]){
  //return 0xD0;
  byte result = 0;
  int sum = 0;

  for (int i = 0; i < (serialSize-1); i++){
    sum += data[i];
  }

  result = sum & 0xFF;

  return result;
}

void writeData(byte data[]){
  digitalWrite(LED_BUILTIN, LOW);
  uint8_t serialPacket[serialSize];
  serialPacket[0] = header;
  for(int i = 0; i < 48; i++){
    serialPacket[i+1] = data[i];
  }
  serialPacket[serialSize-1] = checksum(serialPacket);

  Serial.write(serialPacket, serialSize);
  digitalWrite(LED_BUILTIN, HIGH);
}

