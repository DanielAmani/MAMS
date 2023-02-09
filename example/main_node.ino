/*
Code for ESP32/ESP8266 NRF24 Hub
By: Daniel Amani 
Co: LC28
Github:

*/
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

const unsigned long total_delay = 1000; // Total delay for each loop cycle

/*
Reserved PIN for NRF24
-CSN GPIO 5 / D1
-CE GPIO 4 / D2
-MOSI GPIO 16 / D7
-MISO GPIO 15 / D6
-SCK GPIO 14 / D5
*/
struct data {
    float S_A[3]; //Max 3 type
    float S_B[3]; //Max 3 type
    float S_S[5]; //Max 5 type
    int reed; //reed sensor status for detect if box is open or not
    int type_sensor[3];
};
struct data income;
struct data node1_v;
struct data node2_v;
struct data node3_v;
struct data node4_v;

struct data update;

/*
/Don't know why cannot recived struct from main node so by this time just using array float to control
struct mcu_main {
    float get_value[4];
    uint16_t node_value;
};
struct mcu_main get_new;
*/

float to_node[8];

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
  //Testing Send and recived value
  to_node[1]=100;
  to_node[7]=1000;
  Serial.println(node2_v.S_A[1]);

  RF24NetworkHeader headerMain(node02);   // (Address where the data is going)
    bool ok = network.write(headerMain, &to_node, sizeof(to_node)); // Send the data
     
  delay(total_delay);
  // put your main code here, to run repeatedly:

}
