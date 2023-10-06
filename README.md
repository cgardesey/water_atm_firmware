# Arduino Water ATM Project

This is an Arduino project that demonstrates school alarm system . The project uses the Arduino programming language and can be customised for similar projects

## Table of Contents
- [Features](#features)
- [Requirements](#requirements)
- [Hardware Setup](#hardware-setup)
- [Software Setup](#software-setup)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)


## Features

- Automated water dispensing based on user input and payment.
- Real-time monitoring of water levels.
- Secure payment system integrated with Arduino.
- User-friendly interface for easy interaction.
- Low-cost components for widespread implementation.

## Requirements
To run this project, you will need the following components:
- Push button
- ISD2560
- ATmega32
- 2004 LCD
- 10k pot
- LA4445 audio amp module
- DS1307 RTC
- Cr2032 holder
- Cr2032 coin cell
- 10k resistor
- Capacitors (100uF, 100nF, 22pF)
- 16mhz xtal
- Jumper wires
- Breadboard (optional, for prototyping)

## Hardware Setup
Setup the the hardware as shown below:

![image](https://github.com/cgardesey/water_atm_firmware/assets/10109354/6b4260cd-0c2d-433c-8d43-3ecf96c2ed2c)



## Software Setup
1. Install the Arduino IDE (Integrated Development Environment) from the [official Arduino website](https://www.arduino.cc/en/software).
2. Connect your Arduino board to your computer using a USB cable.
3. Open the Arduino IDE and select the appropriate board and port from the **Tools** menu.
4. Create a new sketch in the Arduino IDE.
5. Copy and paste the code from the `water_atm_firmware.ino` file into your sketch.
6. Save the sketch with a meaningful name.

## Usage
1. Upload the **water_atm_firmware.ino** sketch to the Arduino board by clicking the **Upload** button in the Arduino IDE.
2. The LED connected to the Arduino board should start blinking at a fixed interval.
3. Experiment with the code to and customize to suit your needs.

## Contributing
Contributions to this project are welcome. If you would like to contribute, please follow these steps:
1. Fork the repository.
2. Create a new branch for your feature or bug fix.
3. Make your changes and test them thoroughly.
4. Submit a pull request with a clear description of your changes.

## License
This project is licensed under the [MIT License](https://opensource.org/licenses/MIT). Feel free to modify and distribute this project as needed.
