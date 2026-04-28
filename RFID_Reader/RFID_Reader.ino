#include <SPI.h>
#include <MFRC522.h>

// Pin definitions for ESP32
#define SS_PIN 5
#define RST_PIN 4

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

void setup() {
  Serial.begin(115200);
  SPI.begin(18, 19, 23, 5); // SCK, MISO, MOSI, SS
  mfrc522.PCD_Init(); // Initialize MFRC522
  Serial.println("Place your RFID card near the reader...");
  Serial.println("--------------------------------------");
}

void loop() {
  // Check for new card
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.print("Card UID: ");

  // Print UID in HEX
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }

  Serial.println();
  Serial.println("--------------------------------------");

  mfrc522.PICC_HaltA(); // Stop reading
  delay(1000);
}
