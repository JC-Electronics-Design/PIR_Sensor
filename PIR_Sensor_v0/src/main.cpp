/*
 * PIR Sensor Board MQTT v0
 * Author: Jonathan Caes
 */
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <MQTTClient.h>
#include "ClosedCube_HDC1080.h"
#include <EEPROM.h>
#include "config.h"

#define PIR_PIN 12
#define BAT_PIN A0
#define DONE_PIN 15

//Used Variables
int Eaddress = 0;                                         //EEPROM address
struct {
  uint8_t bssid[6];
  uint8_t channel;
} AP;

int pir, new_pir;
int battery, bat_map;
String s_bat;
char mess_bat[4];
int counter = 0, timeout = 0;
long last_client_millis = 0;
#define CLIENT_DELAY  200           
float temp, rh;                         
String s_temp, s_rh;        
char messTemp[10], messRH[10];
bool falseBSSID = false;

WiFiClient net;
MQTTClient client;

ClosedCube_HDC1080 hdc1080;

void wifi_connect() {
  if(falseBSSID) {
    //Do normal connect and store current bssid and channel
    #ifdef DEBUG
      Serial.println("Connect to wifi (normal)");
    #endif
    WiFi.config(ip, gateway, subnet);               //Wifi configuration
    WiFi.mode(WIFI_STA);                            //Wifi mode
    WiFi.begin(ssid, password);                     //Start connecting to AP
  }
  else {
    //Connect via bssid and channel to AP
    #ifdef DEBUG
      Serial.println("Connect to wifi via bssid");
    #endif
    WiFi.config(ip, gateway, subnet);                       //Wifi configuration
    WiFi.begin(ssid, password, AP.channel, AP.bssid);       //Start connecting to AP
  }

  while(WiFi.status() != WL_CONNECTED) {
    delay(50);
    if(counter > 150 && timeout < 2) {
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      delay(10);
      WiFi.config(ip, gateway, subnet);               //Wifi configuration
      WiFi.mode(WIFI_STA);                            //Wifi mode
      WiFi.begin(ssid, password);                     //Start connecting to AP
      timeout++;
      counter = 0;
    }
    else if(timeout >= 2) {
      #ifdef DEBUG
        Serial.println("DONE");
      #endif
      digitalWrite(DONE_PIN, HIGH);                       //Write done pin high to go back to sleep
      delay(2000);
      digitalWrite(DONE_PIN, LOW);
      delay(2000);
      
    }
    counter++;
  }

  #ifdef DEBUG
    Serial.println("Connected!");
  #endif

  AP.channel = WiFi.channel();
  memcpy(AP.bssid, WiFi.BSSID(), 6);
  EEPROM.put(Eaddress, AP);
  EEPROM.commit();

  #ifdef DEBUG
    Serial.print("Wifi channel: ");
    Serial.println(AP.channel);
    Serial.printf("AP BSSID: %s\n", WiFi.BSSIDstr().c_str());
  #endif
}

void check_bssid() {
  if(AP.bssid[0] == -1) {
    falseBSSID = true;
    return;
  }
  else {
    falseBSSID = false;
  }
  if(AP.bssid[1] == -1) {
    falseBSSID = true;
    return;
  }
  else {
    falseBSSID = false;
  }
  if(AP.bssid[2] == -1) {
    falseBSSID = true;
    return;
  }
  else {
    falseBSSID = false;
  }
  if(AP.bssid[3] == -1) {
    falseBSSID = true;
    return;
  }
  else {
    falseBSSID = false;
  }
  if(AP.bssid[4] == -1) {
    falseBSSID = true;
    return;
  }
  else {
    falseBSSID = false;
  }
  if(AP.bssid[5] == -1) {
    falseBSSID = true;
    return;
  }
  else {
    falseBSSID = false;
  }
}

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    Serial.println("Start");
  #endif
  pinMode(PIR_PIN, INPUT);   
  pinMode(BAT_PIN, INPUT);
  pinMode(DONE_PIN, OUTPUT);
  pinMode(14, INPUT);

  //Init temp & hum sensor 
  hdc1080.begin(0x40);

  //Read PIR Value
  #ifdef DEBUG
    Serial.println("Read PIR");
  #endif
  pir = digitalRead(PIR_PIN);                         //Read the PIR sensor input
  #ifdef DEBUG
    Serial.print("PIR: ");  Serial.println(pir);
  #endif

  //Read EEPROM data
  EEPROM.begin(20);
  delay(1);
  EEPROM.get(Eaddress, AP);
  check_bssid();    //See if there is a correct bssid value

  //Connect to Wifi
  wifi_connect();

  //Connect to MQTT broker
  client.begin(server, net);                          //Begin MQTT service
  delay(10);
  while(!client.connect(mqttDeviceID)) {              //Connect to your mqtt broker
    delay(10);
  }
  
  //Read Battery Value
  battery = analogRead(BAT_PIN);                        //Read battery voltage, 0-1V ADC
  #ifdef DEBUG
    Serial.print("Raw Battery level: ");  Serial.println(battery);
  #endif
  bat_map = map(battery, MIN_BATTERY, MAX_BATTERY, 0, 100);            //Map the ADC value from 0-1023 to 0-100 percent, Set 
  s_bat = String(bat_map);                              //Convert percentage to string
  s_bat.toCharArray(mess_bat, s_bat.length()+1);        //Convert string to char array
  #ifdef DEBUG
    Serial.print("Battery level: ");  Serial.println(bat_map);
  #endif

  //Read Temp & RH values
  rh = hdc1080.readHumidity();                          //Get Relative Humidity
  String s_rh = String(rh);                             //Convert rh to String
  s_rh.toCharArray(messRH, s_rh.length()+1);            //Convert string to char array

  temp = hdc1080.readTemperature();                     //Get temperature
  String s_temp = String(temp);                         //Convert temp to string
  s_temp.toCharArray(messTemp, s_temp.length()+1);      //Convert string to char array

  #ifdef DEBUG
    Serial.print("RH: ");   Serial.println(rh);
    Serial.print("Temperature: ");  Serial.println(temp);
  #endif

  //Send payload to MQTT broker
  // pir ? client.publish(outTopic1, "MOTION") : client.publish(outTopic1, "IDLE");      //Try this next time to see if this works
  pir ? client.publish(outTopic1, "OPEN") : client.publish(outTopic1, "CLOSED");      //Publish pir value                
  client.publish(outTopic2, messTemp);                                                //Publish temperature value
  client.publish(outTopic3, messRH);                                                  //Publish RH value
  client.publish(outTopic4, mess_bat);                                                //Publish battery percentage  

  #ifdef DEBUG
    Serial.println("MQTT messages send");
  #endif
  
  delay(200);

  if(pir == 1 || new_pir == 1) {
    // client.publish(outTopic1, "IDLE");                  //Try this to see if this works                
    client.publish(outTopic1, "CLOSED");                  //Publish pir value                 
    #ifdef DEBUG
      Serial.println("PIR IDLE MQTT message");
    #endif
  }
  
  #ifdef DEBUG
    Serial.println("Go back to sleep");
  #endif
  delay(100);
}

void loop() {
  digitalWrite(DONE_PIN, HIGH);                           //Write done pin high to go back to sleep
  delay(750);
  digitalWrite(DONE_PIN, LOW);
  delay(500);
}
