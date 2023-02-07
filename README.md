# MAMS

## About

MAMS is a easier way to implement multiple type sensor to communicate with NRF24 and LoRa. The main function is for agriculture management and data collection system.

Why? Because I am lazy to make different code for each sensor so by combine it to one main code can reduce the different intergation of sensor.

### How to use it

------------

```C
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
```
Change number on define to change type of sensor. To enable serial monitor value from sensor change `debugmode false` to `debugmode true`. On `this_node = `the number for each node must different.

You also can change NRF24 CE and CSN pin on `RF24 radio(10, 9)` but better don't change to avoid confussion


#### Reserved Pin for sensors
- A0 A1 A4 A5
- D2 D3 D4 D5

#### Reserved Pin for NRF24
 This pin is tested on Arduino Nano
- CSN D9
- CSN D10
- MOSI D11
- MISO D12
- SCK D13

#### List of library being used
- LowPower
- RF24Network
- RF24
- SPI
- DHT
- OneWire
- DallasTemperature
- Adafruit_Sensor
- Adafruit_BME680
- Adafruit_BME280

#### Type of sensor currently supported

Detail for sensor A

|  No  | Type Sensor  | Pin | Store in |
| :----: |:---------------:|:-----:|:-----|
| 1 | Raw Digital | D2 | Raw Value: `update.S_A[1]` |
| 2 | Raw Analog | A0 | Raw Value: `update.S_A[1]` |
| 3 | DHT11 | D2 | Temp: `update.S_A[1]`  Humi: `update.S_A[2]` |
| 4 | DHT22 | D2 | Temp: `update.S_A[1]`  Humi: `update.S_A[2]` |
| 5 | DS18B20 | D2 | Temp: `update.S_A[1]` |
| 6 | HC-SR501 | D2 | Value: `update.S_A[1]`  Bool: `update.ST_A`|
| 7 | TEMT6000 | A0 | Raw Value: `update.S_A[1]`  Percentage: `update.ST_A`|
| 7 | Capasitive Soil | A0 | Raw Value: `update.S_A[1]`  Percentage: `update.ST_A`|

Detail for sensor B

|  No  | Type Sensor  | Pin | Store in |
| :----: |:---------------:|:-----:|:-----|
| 1 | Raw Digital | D3 | Raw Value: `update.S_B[1]` |
| 2 | Raw Analog | A1 | Raw Value: `update.S_B[1]` |
| 3 | DHT11 | D3 | Temp: `update.S_B[1]`  Humi: `update.S_B[2]` |
| 4 | DHT22 | D3 | Temp: `update.S_B[1]`  Humi: `update.S_B[2]` |
| 5 | DS18B20 | D3| Temp: `update.S_B[1]` |
| 6 | HC-SR501 | D3| Value: `update.S_B[1]`  Bool: `update.ST_B`|
| 7 | TEMT6000 | A1| Raw Value: `update.S_B[1]`  Percentage: `update.ST_B`|
| 7 | Capasitive Soil | A1| Raw Value: `update.S_B[1]`  Percentage: `update.ST_B`

For option sensor C

- BME 680
- BME 280
