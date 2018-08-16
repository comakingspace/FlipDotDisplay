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

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "FlipDotDisplay.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <NTPClient.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.de.pool.ntp.org", 7200, 600000);

#define WLAN_SSID       "CoMakingSpace"
#define WLAN_PASS       "........"


#define AIO_SERVER      "comakingcontroller"
#define AIO_SERVERPORT  1883

WiFiClient client;

Adafruit_MQTT_Client  mqtt(&client, AIO_SERVER, AIO_SERVERPORT);

Adafruit_MQTT_Subscribe textTopic = Adafruit_MQTT_Subscribe(&mqtt, "/CommonRoom/FDD/Text");
Adafruit_MQTT_Subscribe binaryTopic = Adafruit_MQTT_Subscribe(&mqtt, "/CommonRoom/FDD/Binary");



// initialize display with 24x16 dots
FlipDotDisplay flip(48,24);

void setup() {
    flip.setTextWrap(false);

    WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
 
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  mqtt.subscribe(&textTopic);
  mqtt.subscribe(&binaryTopic);
}

void loop() {
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &textTopic) {
      char* text = (char*)textTopic.lastread;
      String textToDisplay =  (String)text;
      scrollText(textToDisplay);
    }
    if (subscription == &binaryTopic) {
      //This is senseless
      char* text = (char*)binaryTopic.lastread;
      String textToDisplay =  (String)text;
      scrollText(textToDisplay);
    }
  }

  displayTime();
}

void scrollText(String text){
  flip.setTextSize(1);
  text = "      " + text;
  int len = text.length();
  for(int pos = 0; pos < len ; pos++){
     String subString = text.substring(pos, pos+8);
     
     for(int i = 6; i > 0; i--){
       flip.setCursor(i-3,9);
       flip.clear();
       flip.print(subString);
       flip.fillRect(0,0,3,24,0);
       flip.fillRect(45,0,3,24,0);
       flip.display();
       delay(150);
     }
  }
}


// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;
 
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
 
  Serial.print("Connecting to MQTT... ");
 
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
}

void displayTime(){
  timeClient.update();
  String timeToDisplay = "";
  flip.clear();
  flip.setCursor(9,8);
  if (timeClient.getHours() <= 9) {
    timeToDisplay = "0" + (String)timeClient.getHours() + ":";
  }
  else {
    timeToDisplay = (String)timeClient.getHours() + ":";
  }
  if (timeClient.getMinutes() <= 9) {
    timeToDisplay = timeToDisplay + "0" + (String)timeClient.getMinutes();
  }
  else {
    timeToDisplay = timeToDisplay + (String)timeClient.getMinutes();
  }
  flip.setTextSize(1);
  flip.print(timeToDisplay);
  flip.display();
}

