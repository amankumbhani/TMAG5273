# Texas Instruments - TMAG5273 I2C Library

This is a C++ library to interface TMAG5273, a 3D hall effect sensor over I2C, using an ESP32.

# Files
1. TII2C.cpp: The source file which communicates with the IC on a low level.
2. TII2C.h: The header file containing all the register and function definitions of the class.
3. examples: A folder containing an example code to interface the IC and extract the X, Y & Z magnetic values in mT and temperature in degrees celsius with a reading time of 20 seconds and a sampling period of 100 milliseconds and a sleep time of 40 seconds.
4. includes: A folder that contains the register map of the IC.

## Quick Start

In order to start communicating with the IC over I2C, go to the **examples** directory and under **Basic** folder, open the Basic.ino file using Arduino IDE. Set the board type to ESP32 and configure the required SDA & SCL pins and hit upload. This will start communication with the device.

## General Overview

The sketch first imports the necessary packages from their default Arduino libraries directory. It then creates an object of the TII2C class which can be used to access member variables & member functions of the class to set different parameters like setting the operating mode.
```
#include <TII2C.h>
#include <Wire.h>

#define deviceAddress 0x35		// please refer to datasheet and change to respective address

TII2C tiDevice(deviceAddress);
```

As the next step, the sketch moves on to execute the void setup() function. It first begins the Wire library. Incase you wish to use custom pins for SDA & SCL, you can pass them as variables in the Wire.begin(); function at the beginning of the setup loop. 
```
#define SDA_PIN 21	// default for ESP32
#define SCL_PIN 22	// default for ESP32

void setup() {
	Wire.begin(SDA_PIN, SCL_PIN);
	Wire.setClock(400000);
	.
	.
	.
	.
}
```
Here we also set the clock frequency to 400000 (refer to the datasheet for this value).

Now to begin reading values, simply set the mode to either continuous operation or when not in use, set the operating mode to sleep. You can set the operating mode by changing bits in the respective register field by using the tiDevice object created. DC2 below is the abbrevation for the register you wish to change (DEVICE_CONFIG_2).
```
tiDevice.DEVICE_CONFIG_2_REGFIELD = 0b00000000; // send the device to standby mode
tiDevice.setOperatingMode("DC2");
```

Similarly, all of the functions of the IC can be made use of using the member functions of the library.

## Resources
1. Wire library: https://www.arduino.cc/en/reference/wire
2. TMAG5273 datasheet: https://www.ti.com/product/TMAG5273
