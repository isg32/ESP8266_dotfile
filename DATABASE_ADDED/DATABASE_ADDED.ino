#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <WiFiClientSecure.h>

#define SS_PIN    15  
#define RST_PIN   5   
#define EM18_RX   4   

MFRC522 mfrc522(SS_PIN, RST_PIN);
SoftwareSerial RFID(EM18_RX, -1);
WiFiClientSecure client;  
ESP8266WebServer server(80);

const char* ssid = "CIA_HONEYPOT_2";
const char* password = "8153022369";
const char* githubURL = "https://raw.githubusercontent.com/isg32/ESP8266_dotfile/main/cards.json";
const char* scriptURL = "https://script.google.com/macros/s/AKfycbxu1N6nZc_FFEEXuM5pLog_QdqGaM3mewnFWRzcD1QD2hS9Ixi4pjAoV5_VMFOZtceT9w/exec"; 

String logs = ""; // Store logs for the web UI
DynamicJsonDocument cardDB(2048);

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    String ip = WiFi.localIP().toString();
    Serial.println("\nConnected! IP: " + ip);
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
    if (name == "") {
        logs += "Unknown Card " + uid + "<br>";
        Serial.println("Unknown Card " + uid);
        sendToGoogleSheets("Unknown", uid);
        return;
    }

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
    return "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>ESP8266 Logs</title><meta http-equiv='refresh' content='3'></head><body><h2>ESP8266 Serial Logs</h2><div>" + logs + "</div></body></html>";
}