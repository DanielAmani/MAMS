/*
Code for LoRa-Hub
By: Daniel Amani 
Co: LC28
Github:
TODO: -Adding OLED Screen
      -Two way communication
      -Optimize codee
*/

#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <I2C_Anything.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SCK 13
#define MISO 12
#define MOSI 11
#define SS 10
#define RST 9
#define DIO0 8
#define BAND 433E6

char buff[10];

//433E6 for Asia
//866E6 for Europe
//915E6 for North America

int counter = 0;

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
  Serial.begin(9600);
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x78)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  while (!LoRa.begin(BAND)) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
  
  Wire.begin(8); // Initialize I2C communication as a slave with address 8
  Wire.onReceive(receiveData); //function to be called when data is received
}

void loop() {


  
}

void receiveData(int byteCount) {
  // Read the incoming data into a struct
  I2C_readAnything(update);

  // Display the received data
  // If function don't touch it
  
    Serial.println("");
    Serial.print("Received data: ");
    Serial.print(update.node_id);    
    Serial.println("");
    Serial.print("Value SA-1: ");
    Serial.println(update.S_A[1]);
    Serial.print("Value SA-2: ");
    Serial.println(update.S_A[2]);
    Serial.print("Value SB-1: ");
    Serial.println(update.S_B[1]);
    Serial.print("Value SB-2: ");
    Serial.println(update.S_B[2]);
  
    // Create Char
    char C_SA1 [10];
    char C_SA2 [10];
    char C_SB1 [10];
    char C_SB2 [10];
    char C_SS1 [10];
    char C_SS2 [10];
    char C_SS3 [10];
    char C_SS4 [10];
    char C_SS5 [10];
    // Convert float to string array
    dtostrf(update.S_A[1], 6, 2, C_SA1);
    dtostrf(update.S_A[2], 6, 2, C_SA2);
    dtostrf(update.S_B[1], 6, 2, C_SB1);
    dtostrf(update.S_B[2], 6, 2, C_SB2);
    dtostrf(update.S_S[1], 6, 2, C_SS1);
    dtostrf(update.S_S[2], 6, 2, C_SS2);
    dtostrf(update.S_S[3], 6, 2, C_SS3);
    dtostrf(update.S_S[4], 6, 2, C_SS4);
    dtostrf(update.S_S[5], 6, 2, C_SS5);
    // Adding end char
    strlcat(C_SA1,"|",9);
    strlcat(C_SA2,"|",9);
    strlcat(C_SB1,"|",9);
    strlcat(C_SB2,"|",9);
    strlcat(C_SS1,"|",9);
    strlcat(C_SS2,"|",9);
    strlcat(C_SS3,"|",9);
    strlcat(C_SS4,"|",9);
    strlcat(C_SS5,"|",9);
    // Start sending LoRa messages 
    LoRa.beginPacket();
    LoRa.write(update.node_id);
    LoRa.print(C_SA1);
    LoRa.print(C_SA2);
    LoRa.print(C_SB1);
    LoRa.print(C_SB2);
    LoRa.print(C_SS1);
    LoRa.print(C_SS2);
    LoRa.print(C_SS3);
    LoRa.print(C_SS4);
    LoRa.print(C_SS5);
    LoRa.endPacket();

    Serial.print("String SA-1: ");
    Serial.println(C_SA1);
    Serial.print("String SA-2: ");
    Serial.println(C_SA2);
    Serial.print("String SB-1: ");
    Serial.println(C_SB1);
}
