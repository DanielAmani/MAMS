# MAMS

## About

MAMS is a easier way to implement multiple type sensor to communicate with NRF24 and LoRa. The main function is for agriculture management and data collection system.

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
