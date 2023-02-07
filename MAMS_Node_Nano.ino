/*
Code for MAMS NRF24 child/node
By: Daniel Amani 
Co: LC28
Github:

*/

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
*/

RF24 radio(10, 9);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 02;   // Address of our node in Octal format ( 04,031, etc)
const uint16_t master00 = 00;    // Address of the other node in Octal format

#define sensor_A_type 5
#define sensor_B_type 0
#define sensor_C_type 1
#define debugmode false

#define SEALEVELPRESSURE_HPA (1013.25)//Change sea level pressure if needed
const unsigned long interval = 1000;  //ms  // How often to send data to the other unit
unsigned long last_sent;            // When did we last send?

//Place holder value for save value
struct data {
  float S_A [3]; //Max 3 type
  float S_B [3]; //Max 3 type
  float S_C [5]; //Max 5 type
  bool reed; //reed sensor status
};
struct data update;
 
float new_value[8];

int i;
/*
Type of sensor A
1-DHT11 Data Pin D2 5V | Temp Humidity
2-DHT22 Data Pin D2 5V | Temp Humidity
3-DS18B20 Data Pin D2 5V | Temp
4-HC-SR501 Data Pin D2 5V | Value
5-TEMT6000 Data Pin A1 5V  | Value

Type of sensor C
1-BME680 SDA A4, SCL A5, 5V i2C | Temp Humidity Pressure Altitute Gas
2-BME280 SDA A4, SCL A5, 5V i2C | Temp Humidity Pressure Altitte

Type of sensor Analog
2-Capasitvive Soil Sensor
4=3-HC-SR501 IR
*/
//Using #if exclude code block before compile base on sensor type selection


#if sensor_A_type == 1
  #define DHTpin 2 
  #define DHTtype DHT11
     DHT dht(DHTpin,DHTtype);
#elif sensor_A_type == 2
  #define DHTpin 2 
  #define DHTtype DHT22
  DHT dht(DHTpin,DHTtype);

#elif sensor_A_type == 3
  #define DS18B20pin  2
  OneWire oneWire(DS18B20pin);
  DallasTemperature sensors(&oneWire);
#elif sensor_A_type == 4
  #define PIRpin 2
#elif sensor_A_type == 5
  #define TEMT6000pin A1
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
  network.begin(90, this_node);  //(channel, node address)
  radio.setDataRate(RF24_2MBPS); //Speed
  
 Serial.print("Sensor A selection : ");
 Serial.println(sensor_A_type);
  Serial.print("Sensor C selection : ");
 Serial.println(sensor_C_type);

#if sensor_A_type==1 || sensor_A_type==2
    dht.begin();
    Serial.println("Sensor A Type = DHT");

#elif sensor_A_type==3
    sensors.begin();
    Serial.println("Sensor A Type = DS18B20");

#elif sensor_A_type==4
    pinMode(PIRpin,INPUT);
    Serial.println("Sensor A Type = PIR");   

#elif sensor_A_type==5
    pinMode(TEMT6000pin,INPUT);
    Serial.println("Sensor A Type = TEMT6000"); 
#endif

#if sensor_C_type==1
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
#elif sensor_C_type==2
   Serial.println("Sensor C Type = BME280"); 
  while (!Serial);
  Serial.println(F("BME280 async test"));
  bool status;
  status = bme.begin();  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);(1);
  }
#endif

}

void loop() {
//----------Network NRF24----------//
 network.update();
//----------Recieved NRF24----------//
while ( network.available() ) { 
  RF24NetworkHeader header; 
  Serial.println("Data recieved from main");
  network.read(header, &new_value, sizeof(new_value));
  Serial.print("Value from Main: ");
  Serial.println(new_value[1]);
}
//----------Sending NRF24----------//
  RF24NetworkHeader header2(master00);     // (Address where the data is going)
  bool ok = network.write(header2, &update, sizeof(data)); // Send the data
  

//----------Sensor Type A----------//
#if sensor_A_type==1 || sensor_A_type==2
  update.S_A[1] = dht.readTemperature();
  update.S_A[2] = dht.readHumidity();
  delay(3000);

#elif sensor_A_type==3
  sensors.requestTemperatures(); 
  // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  update.S_A[1] = = sensors.getTempCByIndex(0);
  delay(1000);

#elif sensor_A_type==4
int move = digitalRead(PIRpin);
  if(move==HIGH){
  Serial.println("PIR Movement Detected");
  update.S_A[1] = 200;
  }
  else{
  Serial.println("PIR No Movement");
  update.S_A[1] = 100;
  }
  delay(1000);

#elif sensor_A_type==5
  int light_read = analogRead(TEMT6000pin);
  float light = light_read * 0.0976;
  update.S_A[1]=light;
  delay(1000);
#endif

//----------Sensor Type C----------//
#if sensor_C_type==1
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

  delay(1000);
#endif
#if debugmode==true
#if sensor_C_type==2
  update.S_C[1] = bme.readTemperature();
  update.S_C[2] = bme.readHumidity();
  update.S_C[3] = bme.readPressure() / 100.0F;
  update.S_C[4] = bme.readAltitude(SEALEVELPRESSURE_HPA);
  delay(1000);
#endif

//----------Debug Data----------//
#if sensor_A_type>0
Serial.println("");
Serial.println("Sensor A Reading");
  for(int i=1; i<4; i++){
    Serial.print("Value ");
    Serial.print(i);
    Serial.print(" = ");
    Serial.println(update.S_A[i]);
  }
#endif

#if sensor_C_type>0
Serial.println("");
Serial.println("Sensor C Reading");
  for(int i=1; i<6; i++){
    Serial.print("Value ");
    Serial.print(i);
    Serial.print(" = ");
    Serial.println(update.S_C[i]);
  }
#endif
#endif

}

