# MAMS
-The code is not fully ready yet. Just put here for backup. Only from Child Node to Main Node is done.

## About

**M**ultinode  **A**griculture **M**onitoring **S**ystem (M.A.M.S.) is a easier way to implement multiple type sensor to communicate with NRF24 and LoRa. The main function is for agriculture management and data collection system.

Why? Because I am lazy to make different code for each sensor so by combine it to one main code can reduce the different intergation of sensor.

### Architecture
By using NRF24 for short distance node and LoRa for long distance
![alt text](https://raw.githubusercontent.com/DanielAmani/MAMS/main/img/MAMSV2.png "MAMS Architecture")

### How to use it

------------

```C
RF24 radio(10, 9);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t this_node = 02;   // Address of our node in Octal format ( 04,031, etc)
const uint16_t master00 = 00;    // Address of the other node in Octal format

#define sensor_A_type 8    // Type of sensor A
#define sensor_B_type 7    // Type of sensor B
#define sensor_S_type 1    // Type of sensor S
#define debugmode true    // Serial monitor debug
#define lowmode false    // Enable low power mode
#define minutes_low 1    // Minute of low power

//Sensor value
#define SEALEVELPRESSURE_HPA (1013.25)    // Change sea level pressure if needed for BME680 and BME280
#define TEMT6000_TIMES 0.0976     // Change value to calculate percentage light TEMT6000
#define SOIL_AIR_VALUE 490     // Value for Soil sensor 100% dry
#define SOIL_WATER_VALUE 197    // Value for Soil sensor 100% wet

const unsigned long total_delay = 2000;    // Total delay for each loop cycle
const unsigned long sensor_delay = 100;    // Delay between sensor
const unsigned long interval_send = 10000;    // How often to send data to the main unit
unsigned long last_sent;    // When did we last send?
bool get_data;    //Check if get data from Main Node
```
Change number on define to change type of sensor. To enable serial monitor value from sensor change `debugmode false` to `debugmode true`. On `this_node = `the number for each node must different.

You also can change NRF24 CE and CSN pin on `RF24 radio(10, 9)` but better don't change to avoid confussion. To enable low power mode change `lowmode false` to `lowmode true` and adjust how many minutes that you want arduino on low power. by changing value of `minutes_low`.

For `total_delay` meaning delay for each time it complete main loop while `sensor_delay` is delay for each sensor reading. `interval_send` is for interval NRF24 to send data to main node.

Under `//Sensor value` you can change Define value for some type of sensor for accuracy. Example for soil moisture sensor you need calibrate the 'Dry' and 'Wet' value to ensure you get good result.

To make digital 'ON' and 'OFF' can be transfer within of NRF24 limit transfer, the value `200` meaning **ON/TRUE** while `100` is **OFF/FASLE** . 

#### Reserved Pin for sensors (Node/Child)

This pin is tested on Arduino Nano

- A0 A1 A4 A5
- D2 D3 D4 D5

#### Reserved Pin for NRF24 (Node/Child)

 This pin is tested on Arduino Nano
 
- CSN D9
- CSN D10
- MOSI D11
- MISO D12
- SCK D13

#### Reserved Pin for NRF24+LoRa (Main Node/ESP)

NRF
- CSN GPIO 5 / D1
- CE GPIO 4 / D2
- MOSI GPIO 16 / D7
- MISO GPIO 15 / D6
- SCK GPIO 14 / D5

LORA
- NSS GPIO 0 / D3
- RST GPIO 16 / D0
- DIO1 GPIO 2 / D4
- MOSI GPIO 16 / D7
- MISO GPIO 15 / D6
- SCK GPIO 14 / D5
- GND (any Ground)
- 3.3V

**Note: ESP32/8266 need address by GPIO in code**

#### List of library being used
- LowPower https://github.com/rocketscream/Low-Power
- RF24Network https://nrf24.github.io/RF24Network/
- RF24 https://nrf24.github.io/RF24/
- SPI (Install by default)
- DHT https://github.com/adafruit/DHT-sensor-library
- OneWire https://www.pjrc.com/teensy/td_libs_OneWire.html
- DallasTemperature https://github.com/milesburton/Arduino-Temperature-Control-Library
- Adafruit_Sensor https://github.com/adafruit/Adafruit_Sensor
- Adafruit_BME680 https://github.com/adafruit/Adafruit_BME680
- Adafruit_BME280 https://github.com/adafruit/Adafruit_BME280_Library

Later
- RF24Mesh https://nrf24.github.io/RF24Mesh/ for mesh implimentation
- ESP-DASH https://github.com/ayushsharma82/ESP-DASH for local ESP dashboard (optional)
- WiFiClientSecure (buit-in ESP) for WiFi security
- Wifi Manager https://github.com/tzapu/WiFiManager for config wifi without hardcode SSID
- AsyncTCP https://github.com/dvarrel/AsyncTCP fow webpage implimentation
- Universal Telegram Bot https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot for Telegram intergation
- ArduinoJson https://arduinojson.org/?utm_source=meta&utm_medium=library.properties for JSON

#### Type of sensor currently supported

Change the value of `#define sensor_A_type` , `#define sensor_B_type` and `#define sensor_S_type` to the type of sensor that you want to use based on guide table below.

Example `#define sensor_A_type 3` meaning set sensor A as DHT22. If no sensor selected just change the value to `0`

Detail for sensor A 

|  Num  | Type Sensor  | Pin | Store in |
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

|  Num  | Type Sensor  | Pin | Store in |
| :----: |:---------------:|:-----:|:-----|
| 1 | Raw Digital | D3 | Raw Value: `update.S_B[1]` |
| 2 | Raw Analog | A1 | Raw Value: `update.S_B[1]` |
| 3 | DHT11 | D3 | Temp: `update.S_B[1]`  Humi: `update.S_B[2]` |
| 4 | DHT22 | D3 | Temp: `update.S_B[1]`  Humi: `update.S_B[2]` |
| 5 | DS18B20 | D3| Temp: `update.S_B[1]` |
| 6 | HC-SR501 | D3| Value: `update.S_B[1]`  Bool: `update.ST_B`|
| 7 | TEMT6000 | A1| Raw Value: `update.S_B[1]`  Percentage: `update.ST_B`|
| 7 | Capasitive Soil | A1| Raw Value: `update.S_B[1]`  Percentage: `update.ST_B`

For option sensor S (Special)

|  Num  | Type Sensor  | Pin | Type | Store in |
| :----: |:---------:|:--------------:|:-----:|:-----|
| 1 | BME680 | SCL->A5 SDA->A4 | i2C | Temp: `update.S_S[1]` Humidity: `update.S_S[2]` Pressure: `update.S_S[3]` Altitute: `update.S_S[4]` Gas: `update.S_S[5]` |
| 2 | BME280 | SCL->A5 SDA->A4 | i2C |  Temp: `update.S_S[1]` Humidity: `update.S_S[2]` Pressure: `update.S_S[3]` Altitute: `update.S_S[4]` |

### Trobleshooting

If NRF24 data problem
- Add 100µF capacitor on GND and 3.3V pin of NRF24 module
- Put the NRF24 module away from other module especially from oscillating power supply
- Use NRF24 module with external antenna
- Youtube guide from Electronoobs: [https://www.youtube.com/watch?v=Z7_Cy66Vnrc/](https://www.youtube.com/watch?v=Z7_Cy66Vnrc/ "NRF24 Frustration - Radio module doesn't work?")

## Limitation / Problem

- Struct not very nice or have problem when transfer data from 32bit module to 8bit module
- NRF24 have 32 byte limit payload

## TODO

- LoRa Support
- ESP32 Main Node and LoRa Node
- Types of low power
- More types and quantity sensor support
- Reduce library based on sensor
- Better implementation of transfering value
- Relay, analog output, push button and LED control
- Mesh network for NRF24 node
