#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN    15  // RFID SS Pin
#define RST_PIN   5   // RFID RST Pin
#define EM18_RX   4   // EM18 RX Pin

MFRC522 mfrc522(SS_PIN, RST_PIN);
SoftwareSerial RFID(EM18_RX, -1);
WiFiClientSecure client;  
ESP8266WebServer server(80);
LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x3F LCD I2C Address 0x27

const char* ssid = "OneTwo";
const char* password = "0zerotwo";
const char* githubURL = "https://raw.githubusercontent.com/isg32/ESP8266_dotfile/main/cards.json";
const char* scriptURL = "https://script.google.com/macros/s/AKfycbxu1N6nZc_FFEEXuM5pLog_QdqGaM3mewnFWRzcD1QD2hS9Ixi4pjAoV5_VMFOZtceT9w/exec"; 

String logs = ""; // Store logs for the web UI
DynamicJsonDocument cardDB(2048);

void setup() {
    Serial.begin(115200);
    lcd.begin(16,2);
    lcd.backlight();
    lcd.clear();  // Clear display before writing

    lcd.setCursor(0, 0);
    lcd.print("WiFi Connecting...");
    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    String ip = WiFi.localIP().toString();
    Serial.println("\nConnected! IP: " + ip);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected!");
    lcd.setCursor(0, 1);
    lcd.print("IP: " + ip);
    
    logs += "ESP8266 IP: " + ip + "<br>";

    SPI.begin();
    mfrc522.PCD_Init();
    RFID.begin(9600);
    client.setInsecure();
    fetchCardDatabase();

    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", generateHTML());
    });
    server.begin();
}

void loop() {
    server.handleClient();
    String card = readMFRC522();
    if (card != "") processCard(card);
    
    card = readEM18();
    if (card != "") processCard(card);
    
    delay(500);
}

void fetchCardDatabase() {
    if (WiFi.status() != WL_CONNECTED) return;
    HTTPClient http;
    http.begin(client, githubURL);
    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
        String payload = http.getString();
        DeserializationError error = deserializeJson(cardDB, payload);
        if (!error) {
            logs += "Card Database Loaded!<br>";
            Serial.println("Card Database Loaded!");
        }
    }
    http.end();
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
    card.trim();
    return card.length() ? card : "";
}

void processCard(String uid) {
    String name = matchCard(uid);
    lcd.clear();

    if (name == "") {
        lcd.setCursor(0, 0);
        lcd.print("Unknown Card");
        lcd.setCursor(0, 1);
        lcd.print(uid);
        
        logs += "Unknown Card " + uid + "<br>";
        Serial.println("Unknown Card " + uid);
        sendToGoogleSheets("Unknown", uid);
        return;
    }

    lcd.setCursor(0, 0);
    lcd.print("User: " + name);
    lcd.setCursor(0, 1);
    lcd.print("UID: " + uid);

    String logEntry = name + " (" + uid + ")<br>";
    logs += logEntry;
    Serial.println(logEntry);
    sendToGoogleSheets(name, uid);
}

String matchCard(String uid) {
    for (JsonArray::iterator it = cardDB.as<JsonArray>().begin(); it != cardDB.as<JsonArray>().end(); ++it) {
        if ((*it)["uid"].as<String>() == uid) return (*it)["name"].as<String>();
    }
    return "";
}

void sendToGoogleSheets(String name, String uid) {
    if (WiFi.status() != WL_CONNECTED) return;
    HTTPClient http;
    http.begin(client, scriptURL);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"name\":\"" + name + "\", \"uid\":\"" + uid + "\"}";
    
    int httpResponseCode = http.POST(payload);
    Serial.println("Google Sheets Response: " + String(httpResponseCode));
    http.end();
}

String generateHTML() {
    return "<!DOCTYPE html><html><head>"
           "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>"
           "<title>ESP8266 Logs</title>"
           "<meta http-equiv='refresh' content='3'>"
           "<style>"
           "body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f4; }"
           "h2 { color: #333; }"
           ".log-box { width: 80%; margin: auto; padding: 10px; border: 1px solid #333; background: white; overflow-y: auto; max-height: 300px; }"
           "</style>"
           "</head><body>"
           "<h2>ESP8266 Serial Logs</h2>"
           "<div class='log-box'>" + logs + "</div>"
           "</body></html>";
}
