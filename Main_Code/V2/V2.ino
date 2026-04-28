#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>

// -------------------- WIFI CONFIG --------------------
const char* ssid = "Varun";
const char* password = "Varun@18";

// -------------------- TELEGRAM CONFIG --------------------
const String botToken = "8775180330:AAFc4sFMVqZEZYcVOCi6kqhU4AijsGmIWBM";
const String chatID   = "5472052074";

// -------------------- USER DATA --------------------
String RFID_1 = "E0 23 6F 5F";
String RFID_2 = "E1 FE 07 0F";
String RFID_3 = "13 13 E1 2C";

String NAME_1 = "Sakshi K";
String NAME_2 = "Varun D";
String NAME_3  = "Om A";

uint8_t FP_1 = 2;
uint8_t FP_2 = 3;
uint8_t FP_3 = 0;
// -------------------- RFID CONFIG --------------------
#define SS_PIN 5
#define RST_PIN 4
MFRC522 mfrc522(SS_PIN, RST_PIN);

// -------------------- FINGERPRINT CONFIG --------------------
HardwareSerial fingerSerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

// -------------------- NTP CONFIG --------------------
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800);

// ---------------------- SETUP -------------------------------
void setup() {
  Serial.begin(115200);

  // WiFi
  Serial.println("\n[WiFi] Connecting...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n[WiFi] Connected!");
  Serial.println(WiFi.localIP());

  // Fingerprint
  Serial.println("[Fingerprint] Initializing...");
  fingerSerial.begin(57600, SERIAL_8N1, 16, 17);
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("[Fingerprint] Sensor detected!");
  } else {
    Serial.println("[Fingerprint] NOT detected!");
    while (1);
  }

  // RFID
  Serial.println("[RFID] Initializing...");
  SPI.begin();
  mfrc522.PCD_Init();

  // Time
  timeClient.begin();

  Serial.println("\nSystem Ready — waiting for RFID...");
}

// --------------------- LOOP ------------------------------
void loop() {

  Serial.println("\n[System] Scan RFID Card...");
  delay(2000);

  String rfidUID = getRFID();
  if (rfidUID == "") return;

  if (rfidUID == RFID_1) {
    handleUser(NAME_1, FP_1);
  } else if (rfidUID == RFID_2) {
    handleUser(NAME_2, FP_2);
  } else {
    Serial.println("[RFID] Unknown Card!");

    String msg = "⚠️ Unknown Card Detected\nUID: " + rfidUID +
                 "\nTime: " + getCurrentTime();

    sendToTelegram(msg);
  }
}

// ------------------- RFID FUNCTION --------------------------
String getRFID() {
  if (!mfrc522.PICC_IsNewCardPresent()) return "";
  if (!mfrc522.PICC_ReadCardSerial()) return "";

  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  content.toUpperCase();
  mfrc522.PICC_HaltA();

  Serial.println("[RFID] UID: " + content.substring(1));
  return content.substring(1);
}

// --------------------- USER HANDLER -------------------------
void handleUser(String name, uint8_t fingerID) {

  Serial.println("[System] RFID matched: " + name);
  Serial.println("[System] Scan Fingerprint...");

  int result = waitForFingerprint(fingerID);

  if (result == 1) {
    Serial.println("[Access] Granted to: " + name);

    String msg = "✅ Access Granted\nName: " + name +
                 "\nTime: " + getCurrentTime();

    sendToTelegram(msg);
  }
  else if (result == 0) {
    Serial.println("[Access] Fingerprint NOT matched!");

    sendToTelegram("❌ Fingerprint Mismatch for " + name);
  }
  else {
    Serial.println("[Access] Fingerprint NOT detected!");

    sendToTelegram("❌ Finger not detected for " + name);
  }
}

// -------------------- FINGERPRINT -------------------------
int waitForFingerprint(uint8_t expectedID) {
  unsigned long start = millis();

  while (millis() - start < 8000) {
    int id = getFingerprintID();

    if (id == expectedID) return 1;
    else if (id != -1 && id != expectedID) return 0;
  }

  return -1;
}

int getFingerprintID() {
  if (finger.getImage() != FINGERPRINT_OK) return -1;
  if (finger.image2Tz() != FINGERPRINT_OK) return -1;
  if (finger.fingerFastSearch() == FINGERPRINT_OK) return finger.fingerID;

  return -1;
}

// -------------------- TELEGRAM -------------------------
void sendToTelegram(String message) {

  message.replace(" ", "%20");
  message.replace("\n", "%0A");

  String url = "https://api.telegram.org/bot" + botToken +
               "/sendMessage?chat_id=" + chatID +
               "&text=" + message;

  HTTPClient http;
  http.begin(url);

  int code = http.GET();

  if (code == 200)
    Serial.println("[Telegram] Sent!");
  else
    Serial.println("[Telegram] Error");

  http.end();
}

// ------------------- TIME -------------------------
String getCurrentTime() {
  timeClient.update();
  return timeClient.getFormattedTime();
}
