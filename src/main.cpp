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
BLECharacteristic* pPowerCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Standard Cycling Power Service UUID and Characteristic
#define CYCLING_POWER_SERVICE_UUID         "00001818-0000-1000-8000-00805f9b34fb"
#define CYCLING_POWER_CHARACTERISTIC_UUID  "00002a63-0000-1000-8000-00805f9b34fb"

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
    Serial.println("MAX6675 Thermocouple to Power Meter Emulator");
    
    // Initialize BLE
    BLEDevice::init(DEVICE_NAME);
    
    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    
    // Create the BLE Service (Cycling Power Service)
    BLEService *pService = pServer->createService(CYCLING_POWER_SERVICE_UUID);
    
    // Create a BLE Characteristic for Cycling Power Measurement
    pPowerCharacteristic = pService->createCharacteristic(
                                CYCLING_POWER_CHARACTERISTIC_UUID,
                                BLECharacteristic::PROPERTY_NOTIFY);
    
    // Add descriptor to the characteristic
    pPowerCharacteristic->addDescriptor(new BLE2902());
    
    // Start the service
    pService->start();
    
    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(CYCLING_POWER_SERVICE_UUID);
    
    // Set appearance as Cycling Power Sensor (0x080A)
    pAdvertising->setAppearance(0x080A);
    
    // Add flags
    BLEAdvertisementData advData;
    advData.setFlags(ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
    pAdvertising->setAdvertisementData(advData);
    
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    
    Serial.println("BLE Cycling Power Service started");
    Serial.println("Broadcasting temperature as power data");
    
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
    
    // Convert temperature directly to power value (no multiplication)
    uint16_t powerValue = (uint16_t)(temperature);
    
    // Ensure power value is never 0 (some devices might ignore 0 values)
    if (powerValue == 0) {
        powerValue = 1;  // Set to minimum positive value
    }
    
    // Debug output
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C (Broadcasting as Power: ");
    Serial.print(powerValue);
    Serial.println(" watts)");
    
    // If connected, update the power value
    if (deviceConnected) {
        // Cycling Power Measurement uses a specific format:
        // First 2 bytes are flags
        // Next 2 bytes are instantaneous power
        
        // Simple format (4 bytes for essential power data)
        uint8_t powerData[4];
        
        // Flags field (Little Endian) - For Cycling Power
        // 0x00, 0x00 indicates minimal format with no optional fields
        powerData[0] = 0x00;  // Flags LSB
        powerData[1] = 0x00;  // Flags MSB
        
        // Power value in watts (Little Endian) (LSB first)
        powerData[2] = powerValue & 0xFF;          // LSB
        powerData[3] = (powerValue >> 8) & 0xFF;   // MSB
        
        // Debug output to show bytes being sent
        Serial.print("Sending bytes: [");
        for (int i = 0; i < 4; i++) {
            Serial.print("0x");
            Serial.print(powerData[i], HEX);
            if (i < 3) Serial.print(", ");
        }
        Serial.println("]");
        
        // Send the power data
        pPowerCharacteristic->setValue(powerData, 4);
        pPowerCharacteristic->notify();
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