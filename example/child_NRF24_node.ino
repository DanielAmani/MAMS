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

/*
Reserved PIN for NRF24
-CSN D9
-CE D10
-MOSI D11
-MISO D12
-SCK D13

Type of sensor A B C D
1-Raw Digital D2 D3 | Raw 
2-Raw Analog A0 A1| Float Percentage
3-DHT11 Data Pin D2 D3 5V | Temp Humidity
4-DHT22 Data Pin D2 D3 5V | Temp Humidity
5-DS18B20 Data Pin D2 D3 5V | Temp
6-HC-SR501 Data Pin D2 D3 5V | Bool ST_A
7-TEMT6000 Data Pin A0 A1 5V  | ValueAnalog ValueCalc

Type of sensor S
1-BME680 SDA A4, SCL A5, 5V i2C | Temp Humidity Pressure Altitute Gas
2-BME280 SDA A4, SCL A5, 5V i2C | Temp Humidity Pressure Altitte

//Using #if exclude code block before compile base on sensor type selection
*/

RF24 radio(10, 9); // nRF24L01 (CE,CSN)
RF24Network network(radio); // Include the radio in the network
const uint8_t this_node = 02; // Address of our node in Octal format ( 04,031, etc)
const uint8_t master00 = 00; // Address of the other node in Octal format

#define sensor_A_type 2 // Type of sensor A
#define sensor_B_type 7 // Type of sensor B
//#define sensor_C_type 0 // Type of sensor C
//#define sensor_D_type 0 // Type of sensor D
#define sensor_S_type 1 // Type of sensor C
#define debugmode true // Serial monitor debug
#define lowmode false // Enable low power mode
#define minutes_low 1 // Minute of low power

//Sensor value
#define SEALEVELPRESSURE_HPA (1013.25) // Change sea level pressure if needed for BME680 and BME280
#define TEMT6000_TIMES 0.0976 // Change value to calculate percentage light TEMT6000
//#define SOIL_AIR_VALUE 490 // Value for Soil sensor 100% dry
//#define SOIL_WATER_VALUE 197 // Value for Soil sensor 100% wet
#define ANALOG_MIN_A 490 // Analog min for percentage calculation Sensor A
#define ANALOG_MAX_A 197 // Analog max for percentage calculation Sensor A
#define ANALOG_MIN_B 490 // Analog min for percentage calculation Sensor B
#define ANALOG_MAX_B 197 // Analog max for percentage calculation Sensor B

const unsigned long total_delay = 2000; // Total delay for each loop cycle
const unsigned long sensor_delay = 100; // Delay between sensor
const unsigned long interval_send = 10000; // How often to send data to the main unit
unsigned long last_sent; // When did we last send data
bool get_data; // Check if get data from Main Node

//Place holder value for save value
struct data {
    float S_A[2]; //Max 2 type
    float S_B[2]; //Max 2 type
    float S_S[5]; //Max 5 type
    int type_sensor[3];
    int stat[4];
};

struct data update;
//Place holder value for from node
/*
//Don't know why cannot recived struct from main node so by this time just using array float to control
struct mcu_main {
    float get_value[4];
    uint16_t node_value;
};
struct mcu_main get_new;
*/
  float from_main[8];

//----------Config Pin, Sensor and Library----------//
#if sensor_A_type == 1
  #define RAW_Dpin_A 2
#elif sensor_A_type == 2
  #define RAW_Apin_A A0
#elif sensor_A_type == 3
  #include <DHT.h>
  #define DHTpin_A 2
  #define DHTtype_A DHT11
  DHT dht_A(DHTpin_A, DHTtype_A);
#elif sensor_A_type == 4
  #include <DHT.h>
  #define DHTpin_A 2
  #define DHTtype_A DHT22
  DHT dht_A(DHTpin_A, DHTtype_A);
#elif sensor_A_type == 5
  #include <OneWire.h>
  #include <DallasTemperature.h>
  #define DS18B20pin_A 2
  OneWire oneWire_A(DS18B20pin_A);
  DallasTemperature sensors_A( & oneWire_A);
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
  #include <DHT.h>
  #define DHTpin_B 3
  #define DHTtype_B DHT11
  DHT dht_B(DHTpin_B, DHTtype_B);
#elif sensor_B_type == 4
  #include <DHT.h>
  #define DHTpin_B 3
#define DHTtype_B DHT22
  DHT dht_B(DHTpin_B, DHTtype_B);
#elif sensor_B_type == 5
  #include <OneWire.h>
  #include <DallasTemperature.h>
  #define DS18B20pin_B 3
  OneWire oneWire_B(DS18B20pin_B);
  DallasTemperature sensors_B( & oneWire_B);
#elif sensor_B_type == 6
  #define PIRpin_B 3
#elif sensor_B_type == 7
  #define TEMT6000pin_B A1
#elif sensor_B_type == 8
  #define SOIL_pin_B A1
#endif

#if sensor_S_type == 1
  #include <Adafruit_BME680.h>
  Adafruit_BME680 bme; // Note using I2C protocol
#elif sensor_S_type == 2
  #include <Adafruit_BME280.h>
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

    Serial.print("Sensor B selection : ");
    Serial.println(sensor_B_type);

    Serial.print("Sensor S selection : ");
    Serial.println(sensor_S_type);


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
    #endif

    #if sensor_S_type == 1
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
    #elif sensor_S_type == 2
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
  int SA,SB,SS;

    //----------Network NRF24----------//
    network.update();
    //----------Recieved NRF24----------//
    while (network.available()) {
        RF24NetworkHeader header;
        network.read(header, &from_main, sizeof(from_main));
        get_data = true;
    }
    //----------Sending NRF24----------//
    unsigned long now = millis();
    if (now - last_sent >= interval_send) { // If it's time to send a data, send it!
        last_sent = now;
        RF24NetworkHeader header2(master00); // To main node
        bool ok = network.write(header2, & update, sizeof(data)); // Send the data
    }
    //----------Sensor Type A----------//
    #if sensor_A_type == 1
    SA=100;
    update.type_sensor[1] = SA;
    update.S_A[1] = digitalRead(RAW_Dpin_A);
    if (update.S_A[1] == HIGH) {
        update.S_A[2] = 200;
    } else {
        update.S_A[2] = 100;
    }

    #elif sensor_A_type == 2
    SA=200;
    update.type_sensor[1] = SA;
    update.S_A[1] = analogRead(RAW_Apin_A);
    update.S_A[2] = map(update.S_A[1], ANALOG_MIN_A, ANALOG_MAX_A, 0, 100);

    #elif sensor_A_type == 3 || sensor_A_type == 4
    SA=300;
    update.type_sensor[1] = SA;
    update.S_A[1] = dht_A.readTemperature();
    update.S_A[2] = dht_A.readHumidity();

    #elif sensor_A_type == 5
    SA=500;
    update.type_sensor[1] = SA;
    sensors_A.requestTemperatures();
    // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
    update.S_A[1] = sensors_A.getTempCByIndex(0);

    #elif sensor_A_type == 6
    SA=600;
    update.type_sensor[1] = SA;
    int move_A = digitalRead(PIRpin_A);
    if (move_A == HIGH) {
        update.S_A[1] = 200;
    } else {
        update.S_A[1] = 100;
    }

    #elif sensor_A_type == 7
    SA=700;
    update.type_sensor[1] = SA;
    int light_read_A = analogRead(TEMT6000pin_A);
    update.S_A[1] = light_read_A;
    float light_A = light_read_A * 0.0976;
    update.S_A[2] = light_A;
    #endif

    #if sensor_A_type > 0
    delay(sensor_delay);
    #endif
    //----------Sensor Type B----------//

    #if sensor_B_type == 1
    SB=100;
    update.type_sensor[2] = SB;
    update.S_B[1] = digitalRead(RAW_Dpin_B);
    if (update.S_B[1] == HIGH) {
        update.S_B[2] = 200;
    } else {
        update.S_B[2] = 100;
    }

    #elif sensor_B_type == 2
    SB=200;
    update.type_sensor[2] = SB;
    update.S_B[1] = analogRead(RAW_Apin_B);
    update.S_B[2] = map(update.S_B[1], ANALOG_MIN_B, ANALOG_MAX_B, 0, 100);

    #elif sensor_B_type == 3 || sensor_B_type == 4
    SB=300;
    update.type_sensor[2] = SB;
    update.S_B[1] = dht_B.readTemperature();
    update.S_B[2] = dht_B.readHumidity();

    #elif sensor_B_type == 5
    SB=500;
    update.type_sensor[2] = SB;
    sensors_B.requestTemperatures();
    // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
    update.S_B[1] = sensors_B.getTempCByIndex(1);

    #elif sensor_B_type == 6
    SB=600;
    update.type_sensor[2] = SB;
    int move_B = digitalRead(PIRpin_B);
    if (move_B == HIGH) {
        update.S_B[1] = 200;
    } else {
        update.S_B[1] = 100;
    }

    #elif sensor_B_type == 7
    SB=700;
    update.type_sensor[2] = SB;
    int light_read_B = analogRead(TEMT6000pin_B);
    update.S_B[1] = light_read_B;
    float light_B = light_read_B * 0.0976;
    update.S_B[2] = light_B;
    #endif

    #if sensor_B_type > 0
    delay(sensor_delay);
    #endif

    //----------Sensor Type S----------//

    #if sensor_S_type == 1
    SS=100;
    update.type_sensor[3] = SS;
    unsigned long endTime = bme.beginReading();
    if (endTime == 0) {
        Serial.println(F("Failed to begin reading :("));
        return;
    }
    update.S_S[1] = bme.temperature;
    update.S_S[2] = bme.humidity;
    update.S_S[3] = bme.pressure / 100.0;
    update.S_S[4] = bme.readAltitude(SEALEVELPRESSURE_HPA);
    update.S_S[5] = bme.gas_resistance / 1000.0;

    //It's okay for parallel work to take longer than
    // BME680's measurement time.
    // Obtain measurement results from BME680. Note that this operation isn't
    #elif sensor_S_type == 2
    SS=200;
    update.type_sensor[2] = SS;
    update.S_S[1] = bme.readTemperature();
    update.S_S[2] = bme.readHumidity();
    update.S_S[3] = bme.readPressure() / 100.0;
    update.S_S[4] = bme.readAltitude(SEALEVELPRESSURE_HPA);
    #endif

    delay(total_delay);

    //----------Debug Data----------//
    #if debugmode == true
    Serial.println("");
    Serial.print("Time: ");
    Serial.println(millis());
    #if sensor_A_type > 0
    Serial.println("Sensor A Reading");
    for (int i = 1; i < 3; i++) {
        Serial.print("Value ");
        Serial.print(i);
        Serial.print(" = ");
        Serial.println(update.S_A[i]);
    }
    #endif
    #if sensor_B_type > 0
    Serial.println("");
    Serial.println("Sensor B Reading");
    for (int i = 1; i < 3; i++) {
        Serial.print("Value ");
        Serial.print(i);
        Serial.print(" = ");
        Serial.println(update.S_B[i]);
    }
    #endif
    #if sensor_S_type > 0
    Serial.println("");
    Serial.println("Sensor S Reading");
    for (int i = 1; i < 6; i++) {
        Serial.print("Value ");
        Serial.print(i);
        Serial.print(" = ");
        Serial.println(update.S_S[i]);
    }
    #endif
    
    if (get_data == true) {
      Serial.println("");
        Serial.println("Get data from Main");
        for (int i = 1; i < 9; i++) {
            Serial.print("Value ");
            Serial.print(i);
             Serial.print(" = ");
            Serial.println(from_main[i]);
        }
        get_data = false;
    }

    #endif
}
