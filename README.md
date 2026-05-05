# MQTT-2-RPints_RyouFingIDing_Me
NodedMCU (ESP8266), MFRC522 RFID, Dual YF-S201 Flow Meters and DS18B20 OneWire MQTT Integration with RaspberryPints
---

To set=up MQTT/RPints integration using an ESP8266 microcontroller, Please reference: https://github.com/Garzlok/RaspberryPints/blob/garzlok_rpi/README.md

This sketch will allow a user of a NodeMCU to attach 1 MFRC522 RFID, 2 YF-5201 Flow Meters, and a DS18B20 OneWire Temperature Sensor and integrate with the RandR+ RaspberryPints branch via MQTT.

Current Pin Assignments:

D0

D1 (Flow Meter 1)

D2 (Flow Meter 2)

D3 (RFID RST_PIN)

D4 (DS18B20 Data)

3V3 (RFID 3v30

GND (RFID GND)

D5 (RFID MISO)

D6 (RFID MOSI)

D7 (RFID sck)

D8 (RFID SS_Pin)

RX

TX

GND (Flow Meter GND)

3V3 (Flow Meter 3V3)

---

A0

RSV

RSV

SD3

SD2

SD1

CMD

SD0

CLK

GND

3V3

EN

RST

GND (DS18B20 GND)

VIN (DS18B20 VIN) ---->4.7KOhm<----- (DS18B20 Data)

---


-DS18B20 Sensor will broadcast temperature every 15 min via MQTT to RaspberryPints

-Sensor Payload that RaspberryPints expects:

-"T;probe;temp;unit;timestamp"

-Flow Meters will only broadcast when a pour is initiated

-Minimum of 15 pulses is needed to achieve actual pour (can be adjusted in the sketch)

-Flow Meter Payload that RaspberryPints expects:

-"P;tagIDStatus;tapnumber;pourpulses;RFIDTag"

-tagIDStatus is known as UserID in RaspberryPints

-If no RFID Card is used, tagIDStatus is sent as "-1" which in most cases will not correspond to a user.

-If an RFID Card is used, tagIDStatus is sent as "", which RaspberryPints will determine User ID based on the RFID Tag.

-RFID Information will only be broadcasted in the Flow Meter Payload

-30 seconds after scanning the card, RFID information in RFIDTag will be removed from memory
