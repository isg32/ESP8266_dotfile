#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

/* 📌 **ESP8266 Pin Mapping** */
#define SS_PIN    15  // GPIO15 (D8) - MFRC522 SDA/SS
#define RST_PIN   5   // GPIO5  (D1) - MFRC522 RST
#define SCK_PIN   14  // GPIO14 (D5) - MFRC522 SCK
#define MOSI_PIN  13  // GPIO13 (D7) - MFRC522 MOSI
#define MISO_PIN  12  // GPIO12 (D6) - MFRC522 MISO
#define EM18_RX   4   // GPIO4  (D2) - EM18 TX (Serial Data Output)
#define BUZZER    2   // GPIO2  (D4) - Buzzer Signal Pin

MFRC522 mfrc522(SS_PIN, RST_PIN);
SoftwareSerial RFID(EM18_RX, -1);  // EM18 TX only (RX not used)

void setup() {
    Serial.begin(115200);
    SPI.begin();
    mfrc522.PCD_Init();
    RFID.begin(9600);
    pinMode(BUZZER, OUTPUT);
}

void loop() {
    String card = readMFRC522();
    if (card != "") {
        Serial.println("MFRC522: " + card);
        beepBuzzer();
    }

    card = readEM18();
    if (card != "") {
        Serial.println("EM18: " + card);
        beepBuzzer();
    }

    delay(500);  // Increased delay for card reread
}

String readMFRC522() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return "";
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) uid += String(mfrc522.uid.uidByte[i], HEX);
    mfrc522.PICC_HaltA();
    return uid;
}

String readEM18() {
    String card = "";
    while (RFID.available()) card += (char)RFID.read();
    return card.length() ? card : "";
}

void beepBuzzer() {
    for (int i = 0; i < 2; i++) {
        digitalWrite(BUZZER, 1000);
        delay(200);  // Beep duration
        digitalWrite(BUZZER, 1000);
        delay(200);  // Pause between beeps
    }
}
