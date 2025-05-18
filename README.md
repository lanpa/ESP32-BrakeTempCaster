# ESP32 Brake Temperature Sensor with BLE

This project implements a brake temperature sensor using an ESP32 Node32S development board and a MAX6675 thermocouple interface. The temperature data is broadcast over Bluetooth Low Energy (BLE) as a standard Cycling Power Service, allowing it to be displayed on any cycling computer or app that supports power meters.

## Hardware Requirements

- ESP32 Node32S development board
- MAX6675 thermocouple interface board
- K-type thermocouple
- Jumper wires
- Power source (USB or battery)

## Software Dependencies

The following libraries are used in this project:
- Arduino core for ESP32
- MAX6675 library by Adafruit
- BLE libraries (part of ESP32 Arduino core)

## Getting Started

1. **Install PlatformIO**: Ensure you have PlatformIO installed in your development environment.
2. **Clone the Repository**: Clone this project to your local machine.
3. **Open the Project**: Open the project folder in your IDE with PlatformIO support.
4. **Connect Hardware**: 
   - Connect the MAX6675 thermocouple interface to the ESP32:
     - SCK: GPIO 18
     - CS: GPIO 5
     - MISO: GPIO 19
   - Attach your K-type thermocouple to the MAX6675 board
5. **Build the Project**: Use PlatformIO to build the project.
6. **Upload to ESP32**: Connect your Node32S board and upload the firmware.

## Usage

1. After uploading the firmware, the ESP32 will start broadcasting as a Bluetooth device named "Brake Temperature".
2. Connect to this device using any cycling computer or app that supports power meters.
3. The temperature readings from the thermocouple will be displayed as watts on your cycling computer.
4. Mount the thermocouple on your brake disc or other component you want to monitor.

The temperature data is sent directly as watts (e.g., 85°C will display as 85 watts). This allows you to monitor temperature in real-time during cycling.

## Contributing

Feel free to contribute to this project by submitting issues or pull requests. Your contributions are welcome!