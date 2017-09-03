#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


// Size of the display
const int nDotsX = 24;
const int nDotsY = 16;
// size of one screen refresh
const int DATA_SIZE = nDotsX * nDotsY;

byte dotData[DATA_SIZE/8];
byte options = 0b11111111;



const char* ssid = "";
const char* password = "";

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets


void setup()
{
  for(int i = 0; i < DATA_SIZE/8; ++i)
  {
    dotData[i] = i;
  }

  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  sendFlipData(options, dotData);
}


void sendFlipData(byte const options, byte const data[])
{
  Serial.print(options);
  for(int i = 0; i < DATA_SIZE/8; i++)
  {
    Serial.print(data[i]);
  }
}


void loop()
{
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
    //Serial.println("UDP packet contents:");
    Serial.write(incomingPacket, len);
  }
  if (Serial.available() > 0)
  {
      digitalWrite(2, HIGH);
  }
  else
  {
      digitalWrite(2, LOW);
  }
}
