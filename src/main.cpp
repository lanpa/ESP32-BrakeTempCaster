#include <Arduino.h>
#include <SPI.h>
#include <max6675.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Define pins for MAX6675
#define MAX6675_SCK_PIN 18
#define MAX6675_CS_PIN  5
#define MAX6675_MISO_PIN 19

// Create MAX6675 instance
MAX6675 thermocouple(MAX6675_SCK_PIN, MAX6675_CS_PIN, MAX6675_MISO_PIN);

// BLE Server
BLEServer* pServer = NULL;
BLECharacteristic* pHeartRateCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Standard Heart Rate Service UUID and Characteristic
#define HEART_RATE_SERVICE_UUID        "0000180d-0000-1000-8000-00805f9b34fb"
#define HEART_RATE_CHARACTERISTIC_UUID "00002a37-0000-1000-8000-00805f9b34fb"

// Device name - what will appear on your cycling computer
#define DEVICE_NAME "Brake Temperature"

// Callback for connection events
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Device connected");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Device disconnected");
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("MAX6675 Thermocouple to Heart Rate Emulator");
    
    // Initialize BLE
    BLEDevice::init(DEVICE_NAME);
    
    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    
    // Create the BLE Service (Heart Rate Service)
    BLEService *pService = pServer->createService(HEART_RATE_SERVICE_UUID);
    
    // Create a BLE Characteristic for Heart Rate Measurement
    pHeartRateCharacteristic = pService->createCharacteristic(
                                HEART_RATE_CHARACTERISTIC_UUID,
                                BLECharacteristic::PROPERTY_NOTIFY);
    
    // Add descriptor to the characteristic
    pHeartRateCharacteristic->addDescriptor(new BLE2902());
    
    // Start the service
    pService->start();
    
    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(HEART_RATE_SERVICE_UUID);
    
    // Set appearance as Heart Rate Sensor (0x0340)
    pAdvertising->setAppearance(0x0340);
    
    // Add flags
    BLEAdvertisementData advData;
    advData.setFlags(ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
    pAdvertising->setAdvertisementData(advData);
    
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    
    Serial.println("BLE Heart Rate Service started");
    Serial.println("Broadcasting temperature as heart rate");
    
    delay(500);
}

void loop() {
    // Read temperature in Celsius
    float temperature = thermocouple.readCelsius();
    
    // Check if the reading is valid
    if (isnan(temperature)) {
        Serial.println("Error: Could not read temperature from MAX6675");
        delay(1000);
        return;
    }
    
    // Convert to integer (for heart rate value)
    uint16_t heartRateValue = (uint16_t)temperature;
    
    // Debug output
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C (Broadcasting as HR: ");
    Serial.print(heartRateValue);
    Serial.println(" BPM)");
    
    // If connected, update the heart rate value
    if (deviceConnected) {
        // Option 1: Use UINT8 format (simpler, works for temperatures 0-255°C)
        uint8_t heartRateData[2];
        heartRateData[0] = 0x00;  // Flags: UINT8 format (bit 0 = 0)
        heartRateData[1] = (uint8_t)heartRateValue;  // Temperature as 8-bit value
        
        pHeartRateCharacteristic->setValue(heartRateData, 2);
        pHeartRateCharacteristic->notify();
        
        /* 
        // Option 2: Reverse byte order for UINT16 format
        uint8_t heartRateData[3];
        heartRateData[0] = 0x01;  // Flags: UINT16 format (bit 0 = 1)
        heartRateData[1] = (heartRateValue >> 8) & 0xFF;  // MSB first
        heartRateData[2] = heartRateValue & 0xFF;  // LSB second
        
        pHeartRateCharacteristic->setValue(heartRateData, 3);
        pHeartRateCharacteristic->notify();
        */
    }
    
    // Handle disconnection/reconnection
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // Give time to bluetooth stack
        pServer->startAdvertising(); // Restart advertising
        Serial.println("Started advertising");
        oldDeviceConnected = deviceConnected;
    }
    
    // Handle new connection
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
    
    delay(1000);
}