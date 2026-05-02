/*
NodeMCU (ESP8266)
MFRC522 RFID Only
For Eventual MQTT Integration with RaspberryPints

**WARNING** This sketch attaches the RFID RST Pin to D3 (GPIO0) Which is the FLASH Pin
DO NOT HAVE the RFID RST connected to D3 when flashing this sketch (it WILL NOT Flash)
When uploading the sketch has been completed, you can attach the RST to D3.
This is not an issue during Boot, Only when Flashing! **WARNING**
*/

#include <MFRC522.h>
#include <SPI.h>

void RFIDCardAction(String uid);
void RFIDCheckFunction();

// RFID Settings
#define SS_PIN D8
#define RST_PIN D3
unsigned long lastRfidCheckTime = 0;
unsigned int rfidCheckDelay = 250;
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);

  // RFID
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("RFID Reader Ready");                                 // Init MFRC522 board.
}

void loop() {
  unsigned long now = millis();
  if((now - lastRfidCheckTime) > rfidCheckDelay || lastRfidCheckTime == 0){
    RFIDCheckFunction();
	  lastRfidCheckTime = now;
  }
}

// Function to Read RFID Card and create string variable
void RFIDCheckFunction() {
	 if ( mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() ) {

		  // Save the UID on a String variable
		  String RFIDTag = "";
		  for (byte i = 0; i < mfrc522.uid.size; i++) {
			RFIDTag += String(mfrc522.uid.uidByte[i]);
		  }

		  // calculate BCC
		  byte bcc = 0;
		  for (byte i = 0; i < mfrc522.uid.size; i++) {
			bcc ^= mfrc522.uid.uidByte[i];
		}
		  
		  if (bcc < 0x10) RFIDTag += "0";
		  RFIDTag += String(bcc);

		  RFIDTag.toUpperCase();

      // Pass the UID string to a function
      RFIDCardAction(RFIDTag);

		  // Halt communication with the card
		  mfrc522.PICC_HaltA();
	  }
}


// Function to print UID to see if we have the right card
void RFIDCardAction(String uid) {
  Serial.print("Processing UID: ");
  Serial.println(uid);
  
  if (uid == "22724858213244") { // Replace with your actual UID
    Serial.println("Access Granted");
  } else {
    Serial.println("Access Denied");
  }
}
