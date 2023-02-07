# MAMS

## About

MAMS is a easier way to implement multiple type sensor to communicate with NRF24 and LoRa. The main function is for agriculture management and data collection system.

### How to use it

------------

```C
const uint16_t this_node = 02;   // Address of our node in Octal format ( 04,031, etc)
const uint16_t master00 = 00;    // Address of the other node in Octal format
#define sensor_A_type 5    //Type of sensor A
#define sensor_B_type 0    //Type of sensor B
#define sensor_C_type 1   //Type of sensor C
#define debugmode false    //Debug serial monitor data
```
Change number on define to change type of sensor. To enable serial monitor value from sensor change ```debugmode false``` to ```debugmode true```. On ```this_node = ```the number for each node must different.


#### Type of sensor currently supported
For option sensor A & B
- DHT 11
- DHT 22
- DS18B20
- HC-SR501 (IR)
- TEMT6000
- Raw Analog
- Raw Digital

For option sensor C

- BME 680
- BME 280

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




