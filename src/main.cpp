#include <Arduino.h>
#include <SPI.h>
#include <max6675.h>

// Define pins for MAX6675
#define MAX6675_SCK_PIN 18
#define MAX6675_CS_PIN  5
#define MAX6675_MISO_PIN 19

// Create MAX6675 instance
MAX6675 thermocouple(MAX6675_SCK_PIN, MAX6675_CS_PIN, MAX6675_MISO_PIN);

void setup() {
    Serial.begin(115200);
    Serial.println("MAX6675 Thermocouple Test");
    delay(500);
}

void loop() {
    // Read temperature in Celsius
    float temperature = thermocouple.readCelsius();
    
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    
    // Read temperature in Fahrenheit
    Serial.print("Temperature: ");
    Serial.print(thermocouple.readFahrenheit());
    Serial.println(" °F");
    
    delay(1000);
}