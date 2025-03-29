#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

/* 
📌 ESP8266 (NodeMCU) Pin Mapping for RFID Readers:

✅ **MFRC522 (SPI) Pin Mapping**  
    - **SDA/SS**  → GPIO2  (D4)  
    - **SCK**     → GPIO14 (D5)  
    - **MOSI**    → GPIO13 (D7)  
    - **MISO**    → GPIO12 (D6)  
    - **RST**     → GPIO0  (D3)  
    - **VCC**     → 3.3V  
    - **GND**     → GND  

✅ **EM18 RFID (UART) Pin Mapping**  
    - **TX (Data Out)**  → GPIO4 (D2)  
    - **VCC**            → 5V  
    - **GND**            → GND  

✅ **SPI Connection Notes** (For MFRC522)  
    - ESP8266 **uses hardware SPI** on these pins:  
      - **SCK**  → GPIO14 (D5)  
      - **MOSI** → GPIO13 (D7)  
      - **MISO** → GPIO12 (D6)  
*/

#define SS_PIN 2     // GPIO2  (D4) - MFRC522 SDA/SS
#define RST_PIN 0    // GPIO0  (D3) - MFRC522 RST
#define EM18_RX 4    // GPIO4  (D2) - EM18 TX (Serial Data Output)

// 📌 Initialize Modules
MFRC522 mfrc522(SS_PIN, RST_PIN);  // MFRC522 Instance
SoftwareSerial RFID(EM18_RX, -1);  // EM18 uses TX only, so RX is -1

void setup() {
    Serial.begin(115200);  // Start serial communication
    SPI.begin();           // Initialize SPI for MFRC522
    mfrc522.PCD_Init();    // Initialize MFRC522 module
    RFID.begin(9600);      // Start software serial for EM18 RFID
    Serial.println("RFID readers initialized...");
}

void loop() {
    // Read MFRC522 RFID
    String mfrc522_card = readMFRC522();
    if (mfrc522_card != "") {
        Serial.print("MFRC522 Card UID: ");
        Serial.println(mfrc522_card);
    }

    // Read EM18 RFID
    String em18_card = readEM18();
    if (em18_card != "") {
        Serial.print("EM18 Card Value: ");
        Serial.println(em18_card);
    }

    delay(500);
}

// 📌 Function to Read MFRC522 RFID Card
String readMFRC522() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return "";
    }

    String cardUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        cardUID += String(mfrc522.uid.uidByte[i], HEX);
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return cardUID;
}

// 📌 Function to Read EM18 RFID Card (UART)
String readEM18() {
    if (RFID.available()) {
        String cardValue = "";
        while (RFID.available()) {
            char c = RFID.read();
            if (c != '\n' && c != '\r') {  // Ignore newline characters
                cardValue += c;
            }
        }
        return cardValue;
    }
    return "";
}
