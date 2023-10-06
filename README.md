# Arduino Project: Arduino Water ATM

This is an Arduino project that allows clients to dispense water for users . The project uses the Arduino programming language and can be customised for similar projects

## Table of Contents
- [Features](#features)
- [Requirements](#requirements)
- [Hardware Setup](#hardware-setup)
- [Software Setup](#software-setup)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)


## Features

This project includes the following features:
- **Bluetooth Connectivity**: Hardware connects to android app via Bluetooth for secure and efficient communication.
- **Balance Topup**: Each user has and RFID tag. Clients topup balance on users' RFID tag. On the user screen of android app, client places RFID tag on hardware to launch topup material dialog for balance topup.
- **Register Tag**: Hardware allows clients to register new tags for their users. On the user tag list screen of android app, user places RFID tag on hardware to launch *Register Tag* material dialog to register new tags for users.
- **Voice Propt**: Automated voice promt to welcome and thank users at the start and end of the water vending process respectively.
- **Solar Power**: Solar pawered batteries to keep the hardware system powered

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

If you're looking to integrate with android app project, make sure to check out the the repository corresponding to the [android app](https://github.com/cgardesey/Aquanaut) for detailed instructions.

## License
This project is licensed under the [MIT License](https://opensource.org/licenses/MIT). Feel free to modify and distribute this project as needed.
