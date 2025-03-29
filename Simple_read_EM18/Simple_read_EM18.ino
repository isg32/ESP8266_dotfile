#include <SoftwareSerial.h>

// EM18 Module (UART) Pins
#define EM18_RX D7
#define EM18_TX D6
SoftwareSerial RFID(EM18_RX, EM18_TX);

void setup() {
    Serial.begin(115200);  // Serial monitor
    RFID.begin(9600);      // EM18 works at 9600 baud
    Serial.println("Bring an RFID card near the EM18 reader...");
}

void loop() {
    if (RFID.available()) {
        String tag = "";
        while (RFID.available()) {
            char c = RFID.read();
            tag += c;  // Append characters
        }
        
        tag.trim();  // Remove any leading/trailing spaces
        Serial.print(tag);
    }
}
