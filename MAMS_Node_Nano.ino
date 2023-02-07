/*
Code for MAMS NRF24 child/node
By: Daniel Amani 
Co: LC28
Github:

*/
//Low power library
#include <LowPower.h>
//NRF24 Library
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
//Sensors Library
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <Adafruit_BME280.h>

/*
Reserved PIN for NRF24
-CSN D9
-CE D10
-MOSI D11
-MISO D12
-SCK D13

Type of sensor A &B
1-Raw Digital D2 D3 |
2-Raw Analog A0 A1| Float S_A[1];
3-DHT11 Data Pin D2 D3 5V | Temp Humidity
4-DHT22 Data Pin D2 D3 5V | Temp Humidity
5-DS18B20 Data Pin D2 D3 5V | Temp
6-HC-SR501 Data Pin D2 D3 5V | Bool ST_A
7-TEMT6000 Data Pin A0 A1 5V  | ValueAnalog ValueCalc
8-Capasitive soil sensor A0 A1 5V | ValueAnalog ValueCalc

Type of sensor C
1-BME680 SDA A4, SCL A5, 5V i2C | Temp Humidity Pressure Altitute Gas
2-BME280 SDA A4, SCL A5, 5V i2C | Temp Humidity Pressure Altitte

Type of sensor Analog
2-Capasitvive Soil Sensor
4=3-HC-SR501 IR

//Using #if exclude code block before compile base on sensor type selection
*/

RF24 radio(10, 9);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 02;   // Address of our node in Octal format ( 04,031, etc)
const uint16_t master00 = 00;    // Address of the other node in Octal format

#define sensor_A_type 8  // Type of sensor A
#define sensor_B_type 7  // Type of sensor B
#define sensor_C_type 1    // Type of sensor C
#define debugmode true    //Serial monitor debug
#define lowmode false     //Enable low power mode
#define minutes_low 1    //Minute of low power

//Sensor value
#define SEALEVELPRESSURE_HPA (1013.25)//Change sea level pressure if needed for BME680 and BME280
#define TEMT6000_TIMES 0.0976 //Change value to calculate percentage light TEMT6000
#define SOIL_AIR_VALUE 490 //Value for Soil sensor 100% dry
#define SOIL_WATER_VALUE 197 //Value for Soil sensor 100% wet

const unsigned long total_delay = 2000;    // Total delay for each loop cycle
const unsigned long sensor_delay = 100;    // Delay between sensor
const unsigned long interval = 10000;    // How often to send data to the main unit
unsigned long last_sent;    // When did we last send?
bool get_data;

//Place holder value for save value
struct data {
  float S_A [3]; //Max 3 type
  float S_B [3]; //Max 3 type
  float S_C [5]; //Max 5 type
  bool  ST_A, ST_B, ST_C;// Bool for each sensor
  bool reed; //reed sensor status for detect if box is open or not
  int type_s [3];
};
struct data update;
struct mcu_main {
  float get_value[4];
  bool get_V1,get_V2;
  uint16_t node_value;
};
struct mcu_main get_new;
 
float new_value[8];

int i;

#if sensor_A_type == 1
  #define RAW_Dpin_A 2
#elif sensor_A_type == 2
  #define RAW_Apin_A A0
#elif sensor_A_type == 3
  #define DHTpin_A 2 
  #define DHTtype_A DHT11
  DHT dht_A(DHTpin_A,DHTtype_A);
#elif sensor_A_type == 4
  #define DHTpin_A 2 
  #define DHTtype_A DHT22
  DHT dht_A(DHTpin_A,DHTtype_A);
#elif sensor_A_type == 5
  #define DS18B20pin_A  2
  OneWire oneWire_A(DS18B20pin_A);
  DallasTemperature sensors_A(&oneWire_A);
#elif sensor_A_type == 6
  #define PIRpin_A 2
#elif sensor_A_type == 7
  #define TEMT6000pin_A A0
#elif sensor_A_type == 8
  #define SOIL_pin_A A0
#endif

#if sensor_B_type == 1
  #define RAW_Dpin_B 3
#elif sensor_B_type == 2
  #define RAW_Apin_B A1
#elif sensor_B_type == 3
  #define DHTpin_B 3 
  #define DHTtype_B DHT11
  DHT dht_B(DHTpin_B,DHTtype_B);
#elif sensor_B_type == 4
  #define DHTpin_B 3 
  #define DHTtype_B DHT22
  DHT dht_B(DHTpin_B,DHTtype_B);
#elif sensor_B_type == 5
  #define DS18B20pin_B  3
  OneWire oneWire_B(DS18B20pin_B);
  DallasTemperature sensors_B(&oneWire_B);
#elif sensor_B_type == 6
  #define PIRpin_B 3
#elif sensor_B_type == 7
  #define TEMT6000pin_B A1
#elif sensor_B_type == 8
  #define SOIL_pin_B A1
#endif

#if sensor_C_type == 1
  Adafruit_BME680 bme; // Note using I2C protocol
#elif sensor_C_type == 2
  Adafruit_BME280 bme; // Note using I2C protocol
#endif

void setup() {
    Serial.begin(9600);
    SPI.begin();
    radio.begin();
    network.begin(90, this_node); //(channel, node address)
    radio.setDataRate(RF24_2MBPS); //Speed

    Serial.print("Sensor A selection : ");
    Serial.println(sensor_A_type);
    Serial.print("Sensor C selection : ");
    Serial.println(sensor_C_type);

    #if sensor_A_type == 1
    pinMode(RAW_Dpin_A, INPUT);
    #elif sensor_A_type == 2
    pinMode(RAW_Apin_A, INPUT);
    #elif sensor_A_type == 3 || sensor_A_type == 4
    dht_A.begin();
    Serial.println("Sensor A Type = DHT");

    #elif sensor_A_type == 5
    sensors_A.begin();
    Serial.println("Sensor A Type = DS18B20");

    #elif sensor_A_type == 6
    pinMode(PIRpin_A, INPUT);
    Serial.println("Sensor A Type = PIR");

    #elif sensor_A_type == 7
    pinMode(TEMT6000pin_A, INPUT);
    Serial.println("Sensor A Type = TEMT6000");
    #elif sensor_A_type == 8
    pinMode(SOIL_pin_A, INPUT);
    Serial.println("Sensor A Type = Capasitive Soil Sensor");
    #endif

    #if sensor_B_type == 1
    pinMode(RAW_Dpin_B, INPUT);
    #elif sensor_B_type == 2
    pinMode(RAW_Apin_B, INPUT);
    #elif sensor_B_type == 3 || sensor_B_type == 4
    dht_B.begin();
    Serial.println("Sensor B Type = DHT");

    #elif sensor_B_type == 5
    sensors_B.begin();
    Serial.println("Sensor B Type = DS18B20");

    #elif sensor_B_type == 6
    pinMode(PIRpin_B, INPUT);
    Serial.println("Sensor B Type = PIR");

    #elif sensor_B_type == 7
    pinMode(TEMT6000pin_B, INPUT);
    Serial.println("Sensor B Type = TEMT6000");
    #elif sensor_B_type == 8
    pinMode(SOIL_pin_B, INPUT);
    Serial.println("Sensor B Type = Capasitive Soil Sensor");
    #endif


    #if sensor_C_type == 1
    Serial.println("Sensor C Type = BME680");
    while (!Serial);
    Serial.println(F("BME680 async test"));

    if (!bme.begin()) {
        Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
        while (1);
    }
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150); // 320*C for 150 ms
    #elif sensor_C_type == 2
    Serial.println("Sensor C Type = BME280");
    while (!Serial);
    Serial.println(F("BME280 async test"));
    bool status;
    status = bme.begin();
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
    #endif
    Serial.println("---System Start---");

}

void loop() {
    //----------Network NRF24----------//
    network.update();
    //----------Recieved NRF24----------//
    while (network.available()) {
        RF24NetworkHeader header;
        Serial.println("Data recieved from main");
        network.read(header, & new_value, sizeof(new_value));
        Serial.print("Value from Main: ");
        get_data = true;
        Serial.println(new_value[1]);
    }
    //----------Sending NRF24----------//
    unsigned long now = millis();
    if (now - last_sent >= interval) { // If it's time to send a data, send it!
        last_sent = now;
        RF24NetworkHeader header2(master00); // To main node
        bool ok = network.write(header2, & update, sizeof(data)); // Send the data
    }
    //----------Sensor Type A----------//
    #if sensor_A_type == 1
    update.S_A[1] = digitalRead(RAW_Dpin_A);

    #elif sensor_A_type == 2
    update.S_A[1] = analogRead(RAW_Apin_A);

    #elif sensor_A_type == 3 || sensor_A_type == 4
    update.S_A[1] = dht_A.readTemperature();
    update.S_A[2] = dht_A.readHumidity();

    #elif sensor_A_type == 5
    sensors_A.requestTemperatures();
    // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
    update.S_A[1] = sensors_A.getTempCByIndex(0);

    #elif sensor_A_type == 6
    int move_A = digitalRead(PIRpin_A);
    if (move_A == HIGH) {
        Serial.println("PIR Movement Detected");
        update.S_A[1] = 200;
        update.ST_A == true;
    } else {
        Serial.println("PIR No Movement");
        update.S_A[1] = 100;
        update.ST_A == false;
    }

    #elif sensor_A_type == 7
    int light_read_A = analogRead(TEMT6000pin_A);
    update.S_A[1] = light_read_A;
    float light_A = light_read_A * 0.0976;
    update.S_A[2] = light_A;

    #elif sensor_A_type == 8
    int soil_value_A = analogRead(SOIL_pin_A);
    int soil_percent_A = map(soil_value_A, SOIL_AIR_VALUE, SOIL_WATER_VALUE, 0, 100);
    update.S_A[1] = soil_value_A;
    update.S_A[2] = soil_percent_A;
    #endif

    #if sensor_A_type>0
    delay(sensor_delay);
    #endif

 //----------Sensor Type B----------//

    #if sensor_B_type == 1
    update.S_B[1] = digitalRead(RAW_Dpin_B);

    #elif sensor_B_type == 2
    update.S_B[1] = analogRead(RAW_Apin_B);

    #elif sensor_B_type == 3 || sensor_B_type == 4
    update.S_B[1] = dht_B.readTemperature();
    update.S_B[2] = dht_B.readHumidity();

    #elif sensor_B_type == 5
    sensors_B.requestTemperatures();
    // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
    update.S_B[1] = sensors_B.getTempCByIndex(1);

    #elif sensor_B_type == 6
    int move_B = digitalRead(PIRpin_B);
    if (move_B == HIGH) {
        Serial.println("PIR Movement Detected");
        update.S_B[1] = 200;
        update.ST_B == true;
    } else {
        Serial.println("PIR No Movement");
        update.S_B[1] = 100;
        update.ST_B == false;
    }

    #elif sensor_B_type == 7
    int light_read_B = analogRead(TEMT6000pin_B);
    update.S_B[1] = light_read_B;
    float light_B = light_read_B * 0.0976;
    update.S_B[2] = light_B;

    #elif sensor_B_type == 8
    int soil_value_B = analogRead(SOIL_pin_B);
    int soil_percent_B = map(soil_value_B, SOIL_AIR_VALUE, SOIL_WATER_VALUE, 0, 100);
    update.S_B[1] = soil_value_B;
    update.S_B[2] = soil_percent_B;
    #endif

    #if sensor_B_type>0
    delay(sensor_delay);
    #endif


    //----------Sensor Type C----------//
    #if sensor_C_type == 1
    unsigned long endTime = bme.beginReading();
    if (endTime == 0) {
        Serial.println(F("Failed to begin reading :("));
        return;
    }
    update.S_C[1] = bme.temperature;
    update.S_C[2] = bme.humidity;
    update.S_C[3] = bme.pressure / 100.0;
    update.S_C[4] = bme.readAltitude(SEALEVELPRESSURE_HPA);
    update.S_C[5] = bme.gas_resistance / 1000.0;

    //It's okay for parallel work to take longer than
    // BME680's measurement time.
    // Obtain measurement results from BME680. Note that this operation isn't
    #elif sensor_C_type == 2
    update.S_C[1] = bme.readTemperature();
    update.S_C[2] = bme.readHumidity();
    update.S_C[3] = bme.readPressure() / 100.0 F;
    update.S_C[4] = bme.readAltitude(SEALEVELPRESSURE_HPA);
    #endif

    delay(total_delay);

    //----------Debug Data----------//
    #if debugmode == true    
    Serial.println("");
    Serial.print("Time: ");
    Serial.println(millis());
    #if sensor_A_type > 0
    Serial.println("Sensor A Reading");
    for (int i = 1; i < 4; i++) {
        Serial.print("Value ");
        Serial.print(i);
        Serial.print(" = ");
        Serial.println(update.S_A[i]);
    }
    Serial.print("Bool St_1");
    Serial.print(i);
    Serial.print(" = ");
    Serial.println(update.ST_A);
    #endif
    #if sensor_B_type > 0
    Serial.println("");
    Serial.println("Sensor B Reading");
    for (int i = 1; i < 4; i++) {
        Serial.print("Value ");
        Serial.print(i);
        Serial.print(" = ");
        Serial.println(update.S_B[i]);
    }
    Serial.print("Bool ST_B");
    Serial.print(i);
    Serial.print(" = ");
    Serial.println(update.ST_B);
    #endif    
    #if sensor_C_type > 0
    Serial.println("");
    Serial.println("Sensor C Reading");
    for (int i = 1; i < 6; i++) {
        Serial.print("Value ");
        Serial.print(i);
        Serial.print(" = ");
        Serial.println(update.S_C[i]);
    }
    Serial.print("Bool ST_C");
    Serial.print(i);
    Serial.print(" = ");
    Serial.println(update.ST_C);
    #endif
    if (get_data == true) {
        Serial.println("Get data from Main");
        for (int i = 1; i < 9; i++) {
            Serial.print("Value ");
            Serial.print(i);
            Serial.println(new_value[i]);
        }
        get_data == false;
    }
    #endif
}
