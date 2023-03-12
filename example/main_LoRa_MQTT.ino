#include <Arduino.h>
/*

#if defined(ESP8266)
  // ESP8266 Dependencies 
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
#elif defined(ESP32)
  // ESP32 Dependencies 
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <ESPAsyncWebServer.h>
#endif
#include <ESPDash.h>
#include <RF24Network.h>
#include <RF24.h>
*/

#include <SPI.h>
#include <LoRa.h>
#include <WiFiManager.h>

#define LORA_NSS 15
#define LORA_RST 16
#define LORA_DIO0 4
#define BAND 433E6

//replace the LoRa.begin(---E-) argument with your location's frequency 
//433E6 for Asia
//866E6 for Europe
//915E6 for North America

const unsigned long total_delay = 1000; // Total delay for each loop cycle


struct send {
    uint8_t node_id;
    float S_A[2]; //Max 2 type
    float S_B[2]; //Max 2 type
    float S_S[5]; //Max 5 type
 //   uint8_t main_id;
  //  int type_sensor[3];
  //  int stat[4];
};

struct send update;

void setup() {
    // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    // it is a good practice to make sure your code sets wifi mode how you want it.

    // put your setup code here, to run once:
    Serial.begin(9600);

    
    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
     WiFiManager wm;

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
    // wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
    res = wm.autoConnect("GenericWifi","password123"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        //ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }


  while (!Serial);
  Serial.println("LoRa Receiver");

  //setup LoRa transceiver module
  LoRa.setPins(LORA_NSS,LORA_RST,LORA_DIO0);

  while (!LoRa.begin(BAND)) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");

}

void loop() {
    // put your main code here, to run repeatedly:   
      // try to parse packet
/*
    int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData); 
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
  */
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
   String S_SA1,S_SA2,S_SB1,S_SB2,S_SS1,S_SS2,S_SS3,S_SS4,S_SS5;
    

    update.node_id = LoRa.read();
    S_SA1 = LoRa.readStringUntil('|');
    S_SA2 = LoRa.readStringUntil('|');
    S_SB1 = LoRa.readStringUntil('|');
    S_SB2 = LoRa.readStringUntil('|');
    S_SS1 = LoRa.readStringUntil('|');
    S_SS2 = LoRa.readStringUntil('|');
    S_SS3 = LoRa.readStringUntil('|');
    S_SS4 = LoRa.readStringUntil('|');
    S_SS5 = LoRa.readStringUntil('|');


    Serial.println("");
    Serial.print("Received data: ");
    Serial.print(update.node_id);    
    Serial.println("");
    Serial.print("Value SA-1: ");
    Serial.println(S_SA1);
    //Serial.println(update.S_A[1]);
    Serial.print("Value SA-2: ");
    Serial.println(S_SA2);
    //Serial.println(update.S_A[2]);
    Serial.print("Value SB-1: ");
    Serial.println(S_SB1);
    //Serial.println(update.S_B[1]);
    Serial.print("Value SB-2: ");
    Serial.println(S_SB2);
    //Serial.println(update.S_B[2]);

  }

}
