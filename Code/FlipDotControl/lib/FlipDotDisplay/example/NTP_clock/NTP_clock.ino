/*
 * Example sketch of a NTP clock (needs Wifi connection)
 * 
 * by Patrick Kübler
 * License: Creative Common 4.0
 */
 
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "freifunk-rhein-neckar.de";
const char* password = NULL;

WiFiUDP ntpUDP;


#include <Adafruit_GFX.h>
#include <FlipDotDisplay.h>
FlipDotDisplay flip(24,16);

#include <NTPClient.h>
// [Udp], [Server], [Offset (s)], [Updaterate (milisec)]
NTPClient timeClient(ntpUDP, "0.de.pool.ntp.org", 7200, 600000);


void setup() {
  Serial.begin(115200);
  flip.setTextWrap(false);
  
  flip.clear();  
  flip.print("Wifi...");
  flip.display();
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    
    flip.clear();  
    flip.print("Wifi..");
    flip.display();
    
    delay(300);
    
    flip.clear();  
    flip.print("Wifi...");
    flip.display();
  }

  flip.clear(); 
  flip.print("Wifi..!");
  flip.display();
  
  timeClient.begin();
}

void loop() {
  timeClient.update();

  flip.clear();
  flip.setCursor(0,2);
  flip.print(timeClient.getHours());
  flip.setCursor(13,6);
  if (timeClient.getMinutes() <= 9) {
    flip.drawChar(13,6,0x30,1,0,1);
    flip.setCursor(19,6);
    flip.print(timeClient.getMinutes());
  }
  else {
    flip.print(timeClient.getMinutes());
  }
  flip.display();
  
  delay(1000);
}
