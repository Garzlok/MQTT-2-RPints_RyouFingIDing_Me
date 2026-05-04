#include <MFRC522.h>
#include <SPI.h>

void RFIDCardAction(char* RFIDTag);
void RFIDCheckFunction();

// RFID Settings
#define SS_PIN D8
#define RST_PIN D3
unsigned long lastRfidCheckTime = 0;
unsigned int rfidCheckDelay = 250;
unsigned long lastRfidReadTime;
char RFIDTag[16];
bool tagIsActive = false;
bool messagePrinted = false;
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
 
  // Pass the buffer to a function
  RFIDCardAction(RFIDTag);
}

// Function to Read RFID Card and create string variable
void RFIDCheckFunction() {
	 if ( mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() ) {

		  // Save the UID on a String variable
		  String tempRFIDTag = "";
		  for (byte i = 0; i < mfrc522.uid.size; i++) {
			tempRFIDTag += String(mfrc522.uid.uidByte[i]);
		  }

		  // calculate BCC
		  byte bcc = 0;
		  for (byte i = 0; i < mfrc522.uid.size; i++) {
			bcc ^= mfrc522.uid.uidByte[i];
		}
		  
		  if (bcc < 0x10) tempRFIDTag += "0";
		  tempRFIDTag += String(bcc);

		  tempRFIDTag.toUpperCase();
      tempRFIDTag.toCharArray(RFIDTag, 16);  // Put into buffer

      tagIsActive = true;
      messagePrinted = false;
      lastRfidReadTime = millis();

		  // Halt communication with the card
		  mfrc522.PICC_HaltA();
	  }
}

// Function to print Buffer UID
void RFIDCardAction(char* RFIDTag) {

  // Only print if there is an active tag to process
  if (tagIsActive) {
    
    // Check if 30 seconds have passed
    if (millis() - lastRfidReadTime >= 30000) {
      memset(RFIDTag, 0, 16);
      tagIsActive = false;
      messagePrinted = false;
      Serial.println("Tag memory cleared.");
    } 
    else if (!messagePrinted) {
      // Optional: Only print this if you want to see the tag while it's active
      Serial.print("Processing UID Buffer: ");
      Serial.println(RFIDTag);
      messagePrinted = true;
    }
  }
}
