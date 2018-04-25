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

//#include <gfxfont.h>
#include <Adafruit_GFX.h>
//#include <Adafruit_SPITFT.h>
//#include <Adafruit_SPITFT_Macros.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


const char* ssid = "YOUR_SSID";
const char* password = "PASSWORD";

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

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  writeData(connecting);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    writeData(connecting2);
    delay(300);
    writeData(connecting);
  }

  writeData(wifiOK);

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}

void loop() {

  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    writeData(incomingPacket);

    //wipe chache
    for(int i = 0; i < 255; i++){
      incomingPacket[i] = 0;
    }
  }

  drawLine(5,10,10,10,1);

  /*
  WiFiClient client;

  Serial.printf("\n[Connecting to %s ... ", host);
  if (client.connect(host, 80))
  {
    Serial.println("connected]");

    Serial.println("[Sending a request]");
    client.print(String("GET /") + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n"
                );

    Serial.println("[Response:]");
    while (client.connected())
    {
      if (client.available())
      {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }
    client.stop();
    Serial.println("\n[Disconnected]");
  }
  else
  {
    Serial.println("connection failed!]");
    client.stop();
  }
  delay(5000);
  }
  */

}

void updateDisplay(){
  writeData(drawBuffer);
  for(int i = 0; i<48; i++){
    drawBuffer[i] = B00000000;
  }
}


void drawPixel(uint8_t x, uint8_t y, uint8_t colour){
  bitWrite(drawBuffer[ (3*y) + (x/8) ],
           7-(x%8),
           colour);
}

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

void writeData(uint8_t data[]){
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
