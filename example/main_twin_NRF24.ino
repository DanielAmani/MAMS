#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <Wire.h>
#include <I2C_Anything.h>

//----------NRF24 Config----------//

RF24 radio(10, 9); // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint8_t this_node = 00;   // Address of this node in Octal format ( 04,031, etc)
const unsigned long total_delay = 1000; // Total delay for each loop cycle

struct data {
    float S_A[2]; //Max 2 type
    float S_B[2]; //Max 2 type
    float S_S[5]; //Max 5 type
    int type_sensor[3];
    int stat[4];
};
struct data income;

struct send {
    uint8_t node_id;
    float S_A[2]; //Max 2 type
    float S_B[2]; //Max 2 type
    float S_S[5]; //Max 5 type
  //  uint8_t main_id;
  //  int type_sensor[3];
  //  int stat[4];
};
struct send update;

void setup() {
  Serial.begin(9600);
  Wire.begin(); // Initialize I2C communication as a master
  SPI.begin();
  radio.begin();
  network.begin(90, this_node);  //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
}

void loop() {
  //----------Network NRF24----------//
    network.update();
  //----------Recieved NRF24----------//
    while ( network.available() ) {     // Is there any incoming data?
    RF24NetworkHeader header;
    Serial.println("");
    Serial.print("Found data from: ");
    Serial.println(header.from_node);
    network.read(header, &income, sizeof(data)); // Read the incoming data
//    update.main_id = this_node;
    update.node_id = header.from_node;
    update.S_A[1] = income.S_A[1];
    update.S_A[2] = income.S_A[2];
    update.S_B[1] = income.S_B[1];
    update.S_B[2] = income.S_B[2];

    update.S_S[1] = income.S_S[1];
    update.S_S[2] = income.S_S[2];
    update.S_S[3] = income.S_S[3];
    update.S_S[4] = income.S_S[4];
    update.S_S[5] = income.S_S[5];

/*
    update.type_sensor[1] = income.type_sensor[1];
    update.type_sensor[2] = income.type_sensor[2];
    update.type_sensor[3] = income.type_sensor[3];
    update.stat[1] = income.stat[1];
    update.stat[2] = income.stat[2];
    update.stat[3] = income.stat[3];
    update.stat[4] = income.stat[4];
*/
 
    Serial.println("");
    Serial.print("Value SA-1: ");
    Serial.println(update.S_A[1]);
    Serial.print("Value SA-2: ");
    Serial.println(update.S_A[2]);
    Serial.print("Value SB-1: ");
    Serial.println(update.S_B[1]);
    Serial.print("Value SB-2: ");
    Serial.println(update.S_B[2]);


    Wire.beginTransmission(8); // Address of the receiver Arduino
    I2C_writeAnything (update); // Convert the struct to a byte array and send it
    Wire.endTransmission();
  
   // Serial.write((uint8_t*)&update, sizeof(update));

    }

    Serial.print("X");
    delay(500); // Wait for a bit

}
