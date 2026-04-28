/*
  NodeMCU (ESP8266)
  MFRC522 RFID (Read Only)
  MQTT Integration with RaspberryPints
  RFID Sketch Portion: Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-mfrc522-rfid-reader-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.  
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#include <PubSubClient.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <ESP8266WiFi.h>
#include <time.h>

void setup_wifi();                                               // Add this line at the top
char* getTimestamp();                                            // Add this line at the top
void reconnect();                                                // Add this line at the top
const char* mqtt_topic = "rpints/pours";                         // Add this line at the top
void callback(char* topic, byte* payload, unsigned int length);  // Add this line at the top
void sendUID(const char* rfid, String uidString, const char* timestamp);
const char* rfidUID = "mfrc522.uid->uidByte";

// WiFi Settings
const char* ssid = "SSID";
const char* password = "SSID_PW";

// MQTT Settings
const char* mqtt_server = "raspberrypints.local";  //If your RaspberryPints has a static IP, you can use the IP address.
const int mqtt_port = 1883;
const char* mqtt_user = "RaspberryPints";  //If you change the MQTT user name, make sure you add that name here.
const char* mqtt_pass = "MQTT_PW";

// Learn more about using SPI/I2C or check the pin assigment for your board: https://github.com/OSSLibraries/Arduino_MFRC522v2#pin-layout
MFRC522DriverPinSimple ss_pin(15);

MFRC522DriverSPI driver{ ss_pin };  // Create SPI driver
//MFRC522DriverI2C driver{};     // Create I2C driver
MFRC522 mfrc522{ driver };  // Create MFRC522 instance

WiFiClient espClient;
PubSubClient client(espClient);

char rfidName[24] = "GarageRFID";

void setup() {
  Serial.begin(115200);  // Initialize serial communication
  while (!Serial)
    ;  // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  mfrc522.PCD_Init();                                      // Init MFRC522 board.
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);  // Show details of PCD - MFRC522 Card Reader details.
  Serial.println(F("Scan PICC to see UID"));
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards.
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("Card UID: ");
  MFRC522Debug::PrintUID(Serial, (mfrc522.uid));
  Serial.println();

  // Save the UID on a String variable
  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uidString += "0";
    }
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println(uidString);

  sendUID(rfidName, uidString, getTimestamp());

  // Halt communication with the card
  mfrc522.PICC_HaltA();
}

void setup_wifi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    String clientId = "ESP8266-RFID" + String(rfidUID);
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("Connected!");
      // Subscribe to commands from RPints
      client.subscribe("rpints");
      Serial.println("Subscribed to rpints topic");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

  void callback(char* topic, byte* payload, unsigned int length) {
    // Handle commands from RaspberryPints
    Serial.print("Message received on ");
    Serial.print(topic);
    Serial.print(": ");

    String message = "";
    for (int i = 0; i < length; i++) {
      message += (char)payload[i];
    }

    Serial.println(message);

    // Parse and handle commands here
    // Example: valve control, configuration updates, etc.
  }

  char* getTimestamp() {
    static char buffer[80];
    time_t timer;
    struct tm* timeinfo;

    time(&timer);
    timeinfo = localtime(&timer);


    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", timeinfo);
    return (buffer);
  }

  void sendUID(const char* rfid, String uidString, const char* timestamp) {
    char payload[100];
    snprintf(payload, sizeof(payload),
             "%s;%s;%s",
             rfid,
             uidString,
             timestamp);
    Serial.print("Sending RFID UID: ");
    Serial.println(payload);

    client.publish(mqtt_topic, payload);
  }