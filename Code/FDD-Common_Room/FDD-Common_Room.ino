/*
 * Program running on the FlipDotDisplay
 * in the CoMakingSpace Common Room.
 * 
 * Functions
 * - Display Time via NTP
 * - Text scroll via MQTT trigger
 * - Display Binary via MQTT (kind of works, but not really well)
 *   - one needs to increase the character limit of the Adafruit MQTT library to 144+
 * 
 * by Patrick Kübler
 * License: Creative Commons 4.0
 */

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "FlipDotDisplay.h"
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <NTPClient.h>
#include <TaskScheduler.h>
#include "Passwords.h"
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.de.pool.ntp.org", 7200, 600000);

#define AIO_SERVER      "comakingcontroller"
#define AIO_SERVERPORT  1883

WiFiClient client;

Adafruit_MQTT_Client  mqtt(&client, AIO_SERVER, AIO_SERVERPORT);

Adafruit_MQTT_Subscribe textTopic = Adafruit_MQTT_Subscribe(&mqtt, "/CommonRoom/FDD/Text");
Adafruit_MQTT_Subscribe binaryTopic = Adafruit_MQTT_Subscribe(&mqtt, "/CommonRoom/FDD/Binary");



// initialize display with 24x16 dots
FlipDotDisplay flip(48,24);

void displayTime();
void MQTT_update();

void binaryData(char* data, uint16_t length){
  flip.clear();
  
  uint8_t tempArray[144] = {0};
  
  for (int i = 0; i < length; i++){
    tempArray[i] = data[i];
  }
  
  flip.writeData(tempArray);
  delay(1000);
}

void textData(char* data, uint16_t length){
  scrollText((String)data);
}

// ### Task Scheduler ###
Task timeTask(1000, TASK_FOREVER, &displayTime);
Task MQTTTask(1000, TASK_FOREVER, &MQTT_update);

Scheduler runner;

void setup() {
  Serial.println("Booting");
  flip.print("Booting...");
  flip.display();

// ### WiFi Setup ####
  
  WiFi.mode(WIFI_STA);

  WiFi.hostname("FlipDotDisplay");
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }


// ### OTA Setup ####
  ArduinoOTA.setHostname("FlipDotDisplay");

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]


  //No authentication by default
  ArduinoOTA.setPassword(OTA_password);

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  flip.setTextWrap(false);


// ### MQTT Setup ###
  binaryTopic.setCallback(binaryData);
  textTopic.setCallback(textData);

  mqtt.subscribe(&textTopic);
  mqtt.subscribe(&binaryTopic);

// ### Initialize Tasks ###
  runner.init();
  runner.addTask(timeTask);
  runner.addTask(MQTTTask);
  timeTask.enable();
  MQTTTask.enable();
}

void loop() {
  
  ArduinoOTA.handle();
  
  runner.execute();

}

void MQTT_update(){
  MQTT_connect();
  mqtt.processPackets(1000);
//  Adafruit_MQTT_Subscribe *subscription;
//  while ((subscription = mqtt.readSubscription(1000))) {
//   
//    if (subscription == &textTopic) {
//      char* text = (char*)textTopic.lastread;
//      String textToDisplay =  (String)text;
//      scrollText(textToDisplay);
//    }
//    if (subscription == &binaryTopic) {
//      //Theoretically, the MQTT library returns the data as uint8_t
//      //Therefore, just passing it to the writeData function should work.
//      //However, this was not yet tested
//      //Especially the boundaries of the Adafruit library (SUBSCRIPTIONDATALEN) might have to be adjusted
//      //flip.clear();
//      //flip.writeData(binaryTopic.lastread);
//      //delay(150);
//    }
//  }
}

void scrollText(String text){
  // The Adafruit GFX library tends to do
  // weird things when a letter is going
  // partially of screen. Therefore the 
  // scrolltext is implemented via seperate
  // chars. This also allows for easier 
  // control of custom characters.
  
  // The spaces are added for smooth lead-in
  text = "       " + text;
  int len = text.length();

  for(int i = 0; i < text.length(); i++) {
    char character = text.charAt(i);
      switch( character) {
       case char('ä'): text.setCharAt(i, char(0x84)); text.remove(i-1,1); break;
       case char('ö'): text.setCharAt(i, char(0x94)); text.remove(i-1,1); break;
       case char('ü'): text.setCharAt(i, char(0x81)); text.remove(i-1,1); break;
       case char('Ä'): text.setCharAt(i, char(0x8E)); text.remove(i-1,1); break;
       case char('Ö'): text.setCharAt(i, char(0x99)); text.remove(i-1,1); break;
       case char('Ü'): text.setCharAt(i, char(0x9A)); text.remove(i-1,1); break;
       case char('ß'): text.setCharAt(i, char(0xE0)); text.remove(i-1,1); break;
       //case char(0xC3): text.remove(i-1); break;
      }
  }

  
  for(int pos = 0; pos < len; pos++) {
    
    for(int shift = 0; shift < 6; shift++){
      flip.clear();
      for(int letter = 0; letter < 8; letter++){
        if (pos+letter < len){
          //Since the default font does strange things with german umlauts, we have to replace them.
          //the replacement chars can be found here:
          //https://github.com/adafruit/Adafruit-GFX-Library/blob/master/glcdfont.c
          //Take the line number of the character, substract 19 and convert it to hex
          
          
          flip.drawChar(-shift+6*letter, 8, text.charAt(pos+letter),1,0,1);
        }
      }
      flip.display();
      delay(120);
    }
  }
  flip.drawChar(0,0,text.charAt(0),1,0,1);
  
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
