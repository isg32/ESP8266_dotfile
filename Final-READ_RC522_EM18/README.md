## Overview

This project utilizes the ESP8266 WiFi module to read RFID cards using MFRC522 and EM18 RFID readers and logs scanned card details on a web page.

## Features

- Supports both MFRC522 (SPI-based) and EM18 (serial-based) RFID readers.
    
- Logs scanned RFID card IDs.
    
- Provides a simple web interface to display scanned cards in real time.
    
- Connects to WiFi and allows remote access.
    

## Hardware Requirements

- ESP8266 (NodeMCU or similar)
    
- MFRC522 RFID Module
    
- EM18 RFID Reader
    
- RFID Cards or Tags
    
- Jumper wires
    

## Wiring

|Component|ESP8266 Pin|
|---|---|
|MFRC522 SS|D8 (GPIO15)|
|MFRC522 RST|D1 (GPIO5)|
|MFRC522 SCK|D5 (GPIO14)|
|MFRC522 MOSI|D7 (GPIO13)|
|MFRC522 MISO|D6 (GPIO12)|
|EM18 RX|D2 (GPIO4)|

## Installation & Setup

1. Install the **Arduino IDE** and add the **ESP8266 Board Package**.
    
2. Install the required libraries:
    
    - `ESP8266WiFi.h`
        
    - `ESP8266WebServer.h`
        
    - `MFRC522.h`
        
    - `SoftwareSerial.h`
        
    - `SPI.h`
        
3. Update the WiFi credentials in the code:
    
    ```cpp
    const char* ssid = "Your_WiFi_Name";
    const char* password = "Your_WiFi_Password";
    ```
    
4. Upload the code to your ESP8266 board.
    
5. Open the Serial Monitor at **115200 baud** to view logs.
    

## How to Use

1. **Connect ESP8266 to power.**
    
2. **Scan an RFID card using either MFRC522 or EM18.**
    
3. **Access the web interface:**
    
    - Once the ESP8266 is connected to WiFi, note its **IP address** from the Serial Monitor.
        
    - Open a web browser and enter: `http://<ESP8266_IP_ADDRESS>/`
        
4. **View logs on the web page.**
    
    - The logs update automatically every second.
        

## Troubleshooting

- **ESP8266 not connecting to WiFi?**
    
    - Ensure correct WiFi credentials.
        
    - Try restarting the ESP8266.
        
- **No RFID data?**
    
    - Check wiring and power connections.
        
    - Make sure RFID modules are working.
        
