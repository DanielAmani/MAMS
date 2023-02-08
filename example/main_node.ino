//Code for ESP32/ESP8266 NRF24 Hub

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

*/

//Main
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

//----------NRF24 Config----------//
RF24 radio(4,5);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 00;   // Address of this node in Octal format ( 04,031, etc)
//List of other node
const uint16_t node01 = 01;      // Address of the other node in Octal format
const uint16_t node02 = 02;
const uint16_t node03 = 03;

/*
Reserved PIN for NRF24
-CSN GPIO 5 / D1
-CE GPIO 4 / D2
-MOSI GPIO 16 / D7
-MISO GPIO 15 / D6
-SCK GPIO 14 / D5
*/
struct data {
  float S_A [3]; //Max 3 type
  float S_B [3]; //Max 3 type
  float S_C [5]; //Max 5 type
  bool reed; //reed sensor status
};
struct data income;
struct data node1_v;
struct data node2_v;
struct data node3_v;
struct data node4_v;

struct data update;

struct mcu_main {
  float get_value[4];
  bool get_V1,get_V2;
  uint16_t node_value;
};
struct mcu_main get_new;

float new_value[8];

void setup() {
  Serial.begin(9600);
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
  // put your setup code here, to run once:

}

void loop() {
  //----------Network NRF24----------//
    network.update();
  //----------Recieved NRF24----------//
    while ( network.available() ) {     // Is there any incoming data?
    RF24NetworkHeader header;
    Serial.print("Found data from: ");
    Serial.println(header.from_node);
    network.read(header, &income, sizeof(data)); // Read the incoming data
    if (header.from_node ==  node01) {    // If data comes from Node 02
      node1_v=income;
    }
    if (header.from_node == node02) {    // If data comes from Node 012
      node2_v=income;
    }
  }  
  Serial.println(node1_v.S_A[1]);
  Serial.println(node2_v.S_A[1]);
  
  new_value[1]=200;

  RF24NetworkHeader headerMain(node02);   // (Address where the data is going)
    bool ok = network.write(headerMain, &new_value, sizeof(new_value)); // Send the data  
    Serial.print("Value to Node: ");
  Serial.println(new_value[1]);
  delay(1000);
  // put your main code here, to run repeatedly:

}
