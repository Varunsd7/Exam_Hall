#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <LCDIC2.h>

// -------------------- WIFI CONFIG --------------------
const char* ssid = "Varun";
const char* password = "Varun@18";

// -------------------- TELEGRAM CONFIG --------------------
const String botToken = "8775180330:AAFc4sFMVqZEZYcVOCi6kqhU4AijsGmIWBM";
const String chatID   = "5472052074";

// -------------------- USER DATA --------------------
//------ RFID CARD UID ----------
String RFID_1 = "E0 23 6F 5F";
String RFID_2  = "E1 FE 07 0F";

//------ USER NAME ----------
String NAME_1  = "Sakshi K";
String NAME_2  = "Fariyal J";
String NAME_3  = "Om J";

//------ FINGER PRINT ID ----------
uint8_t FP_1  = 2;
uint8_t FP_2 = 3;
uint8_t FP_3 = 1;
// -------------------- RFID CONFIG --------------------
#define SS_PIN 5
#define RST_PIN 4
MFRC522 mfrc522(SS_PIN, RST_PIN);

// -------------------- FINGERPRINT CONFIG --------------------
HardwareSerial fingerSerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

// -------------------- LCD CONFIG --------------------
LCDIC2 lcd(0x27, 16, 2);

// -------------------- NTP CONFIG --------------------
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800); // UTC+5:30 (India)


// ---------------------- INIT CODE -------------------------------
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  lcd.begin();
  //lcd.int();
  //lcd.backlight();

  // -------------------- WIFI INIT --------------------
  lcd.clear();
  lcd.print("WiFi: Connecting");
  Serial.println("\n[WiFi] Searching for network...");

  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
    Serial.print(".");
    lcd.setCursor(0, 1);
    lcd.print("Connecting...");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WiFi] Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    lcd.clear();
    lcd.print("WiFi: Connected");
    delay(1000);
  } else {
    Serial.println("\n[WiFi] Connection FAILED");
    lcd.clear();
    lcd.print("WiFi Failed!");
    delay(2000);
  }

  // -------------------- FINGERPRINT INIT --------------------
  Serial.println("[Fingerprint] Initializing sensor...");
  lcd.clear();
  lcd.print("Init Fingerprint");

  fingerSerial.begin(57600, SERIAL_8N1, 16, 17);
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("[Fingerprint] Sensor detected!");
    lcd.clear();
    lcd.print("FP: Detected");
  } else {
    Serial.println("[Fingerprint] Sensor NOT detected!");
    lcd.clear();
    lcd.print("FP: Not Detected");
    while (1);
  }
  delay(1500);

  // -------------------- RFID INIT --------------------
  Serial.println("[RFID] Initializing MFRC522...");
  lcd.clear();
  lcd.print("Init RFID Reader");
  SPI.begin();
  mfrc522.PCD_Init();

  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  if (v == 0x92 || v == 0x91) {
    Serial.println("[RFID] MFRC522 detected!");
    lcd.clear();
    lcd.print("RFID: Detected");
  } else {
    Serial.println("[RFID] MFRC522 NOT detected!");
    lcd.clear();
    lcd.print("RFID: Not Found");
    while (1);
  }
  delay(1500);

  // -------------------- NTP INIT --------------------
  timeClient.begin();
  timeClient.update();
  Serial.println("[NTP] Time client started.");

  // -------------------- SYSTEM READY --------------------
  lcd.clear();
  lcd.print("System Ready");
  Serial.println("\nSystem Ready — waiting for RFID...");
  delay(1500);
  lcd.clear();
}

// --------------------- MAIN CODE --------------------------------
void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Scan RFID Card  ");
  delay(5000);

  String rfidUID = getRFID();
  if (rfidUID == "") return;

  if (rfidUID == RFID_1) {
    handleUser(NAME_1, FP_1);
  } else if (rfidUID == RFID_2) {
    handleUser(NAME_2, FP_2);
  } else {
    // Unknown/extra RFID
    lcd.clear();
    lcd.print("Invalid RFID");
    Serial.println("[RFID] Unknown card detected: " + rfidUID);
    String msg = "⚠️ Access Denied – Unknown Card / Possible Fake Access\nUID: " + rfidUID +
                 "\nTime: " + getCurrentTime();
    sendToTelegram(msg);
    delay(2000);
    lcd.clear();
  }
}

// ------------------- RFID FUNCTION ----------------------------------
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
  delay(500);

  Serial.println("[RFID] Card detected: " + content.substring(1));
  return content.substring(1);
}

// --------------------- USER HANDLER --------------------------------

void handleUser(String name, uint8_t fingerID) {
  lcd.clear();
  lcd.print("RFID Matched");
  Serial.println("[System] RFID matched: " + name);
  delay(1000);

  lcd.clear();
  lcd.print("Scan Fingerprint");
  Serial.println("[System] Waiting for fingerprint...");

  int result = waitForFingerprint(fingerID);

  if (result == 1) {
    // Fingerprint matched
    lcd.clear();
    lcd.print("Access Granted");
    lcd.setCursor(0, 1);
    lcd.print("Welcome " + name);
    Serial.println("[Access] Granted to: " + name);
    String msg = "✅ Access Granted in Exam Hall 101\nName: " + name +
                 "\nID: " + String(fingerID) +
                 "\nTime: " + getCurrentTime();
    sendToTelegram(msg);
    delay(3000);
  }
  else if (result == 0) {
    // Fingerprint mismatched
    lcd.clear();
    lcd.print("Invalid ID");
    Serial.println("[Access] Denied for: " + name + " (Fingerprint Not Matched)");
    String msg = "❌ Access Denied – Fingerprint Not Matched\nName: " + name +
                 "\nTime: " + getCurrentTime();
    sendToTelegram(msg);
    delay(2000);
    lcd.clear();
    lcd.print("Try Again...");
    delay(2000);
  }
  else if (result == -1) {
    // Fingerprint not detected
    lcd.clear();
    lcd.print("Finger Not Detected");
    Serial.println("[Access] Denied for: " + name + " (Fingerprint Not Detected)");
    String msg = "❌ Access Denied – Fingerprint Not Detected\nName: " + name +
                 "\nTime: " + getCurrentTime();
    sendToTelegram(msg);
    delay(2000);
    lcd.clear();
    lcd.print("Try Again...");
    delay(2000);
  }

  lcd.clear();
}

// -------------------- FINGERPRINT HANDLER ---------------------------------
// Returns: 1 = matched, 0 = mismatched, -1 = not detected
int waitForFingerprint(uint8_t expectedID) {
  unsigned long start = millis();
  while (millis() - start < 8000) {
    int id = getFingerprintID();
    if (id == expectedID) return 1;      // matched
    else if (id != -1 && id != expectedID) return 0; // mismatched
  }
  return -1; // not detected after timeout
}

int getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) return finger.fingerID;
  return -1;
}

// --------------------- TELEGRAM FUNCTION ---------------------------
void sendToTelegram(String message) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[Telegram] WiFi not connected, skipping send.");
    return;
  }

  // URL encode for Telegram
  message.replace(" ", "%20");
  message.replace("\n", "%0A");
  message.replace("&", "%26");

  String url = "https://api.telegram.org/bot" + botToken +
               "/sendMessage?chat_id=" + chatID +
               "&text=" + message;

  HTTPClient http;
  http.begin(url);
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200)
    Serial.println("[Telegram] ✅ Message sent!");
  else
    Serial.println("[Telegram] ❌ Send error: " + String(httpResponseCode));

  http.end();
}

// ------------------- TIME FUNCTION ------------------------
String getCurrentTime() {
  timeClient.update();
  return timeClient.getFormattedTime();
}
