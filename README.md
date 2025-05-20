# All the Code is Coded by AI

My experiment in coding with pure AI.

One day, my disc brake failed when I was riding down the mountain. I felt that the handle was soft and the braking power was not strong enough. Therefore, I had an idea that I wanted to measure the temperature of the braking system. For the temperature of the system, I could use a remote sensing method like an infrared sensor, but sadly, the emissivity of the disc is not easy to calibrate. So I chose to use a touch-based measuring method based on thermocouple, attached to the brake pad. To record the values, I had some choices like writing the measured values into the flash of the embedded device. But there is a drawback to this method because if I want to render the values onto the video, I have to be able to sync the time of the recorded value and the time of the video frames. After some thought experiments, I decided not to choose this way. Instead, I already made some cycling videos that stamp the speed, cadence, and heart rate based on the cycling computer onto the video. So it narrowed down to broadcasting the temperature signal to a cycling app or my cycling computer.

## First Try
The cadence sensor is fixed on the bike, so I used my heart rate sensor at hand as a reference device. To start, the prompt to code was as simple as:

`I want to use ESP32 device to get the temperature from MAX6675 chip and broadcast the temperature value as heart rate value through BLE device and the broadcast signal should be able to read by cycling apps like Strava or Runkeeper`

The result was pretty amazing. It compiled successfully and was able to print the temperature on the console. I spent most of the time setting up the toolchain. Then I gradually prompted to add BLE broadcast and debug unexpected behavior. However, only Runkeeper could get the value. Other apps couldn't. I guess that's some protocol-based issue. I prompted AI to solve this problem; it tried several different implementations, but the result was the same. Beside this, Runkeeper does not save HR values frequently enough (~30 secs). I had to find another solution.

## Second Try
It seemed that heart rate was not a good way to go. It came to my mind that I could fake it as a power meter. The value range for a power meter is from 0 to thousands of watts, compared to a heart rate sensor, which by default uses a byte to represent the value. The power meter seemed to be a better choice. Although Strava and Runkeeper do not accept power values by design, I found that Wahoo APP is able to receive the power values for each second. All of the data path was now clear.

It was time to put everything together and have a field test.

## The Result
I already knew there were two failing modes. One is fading and the other is gradual locking. Once the brake fades, switching to the rear brake and letting the front caliper cool down makes everything return to normal. When the other case happened, it felt like the brake was engaged automatically with stronger and stronger force (maybe the boiled oil is pushing against the cylinder). The bike can not move even if I tried to pedal on a -20% slope. After stepping off the bike, the feel of heat radiating from the caliper and the burning marks on the disc are noticeable.




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