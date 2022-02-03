/**
   @author Aman Kumbhani
*/
#include "TII2C.h"
#include <Wire.h>

#define WAIT_PERIOD_POST_READ 40000 //< the time in milliseconds till which you wish to read data
#define SAMPLING_PERIOD 100     //< the time in milliseconds at which the data will be requested from the chip
#define READ_PERIOD 20000       //< the period till which you need the X, Y & Z readings & temperature values
#define deviceAddress 0x35      //< the device address with which you wish to communicate

TII2C tiDevice(deviceAddress);  //< creating an object of TIIC type to access member variables and member functions

/**
    @brief A function that is executed only once in an instance
*/
void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
}

/**
    @brief A function that is executed continuously
*/
void loop() {
  float time_now_overall = millis();                // get the current time
  unsigned long startMillis = millis();             // get the current time
  unsigned long currentMillis;                      // declare a variable in which the currentTime will be stored during execution // of loops
  while (millis() < time_now_overall + READ_PERIOD)
  {
    // continuous read mode for 20 seconds
    tiDevice.DEVICE_CONFIG_2_REGFIELD = 0b00000000; // send the device to standby mode
    tiDevice.setOperatingMode("DC2");
    tiDevice.DEVICE_CONFIG_2_REGFIELD = 0b00000010; // continuous measure mode!
    tiDevice.setOperatingMode("DC2");
    currentMillis = millis();  // get the current "time" (actually the number of milliseconds since the program started)
    if (currentMillis - startMillis >= SAMPLING_PERIOD)  // test whether the period has elapsed
    {
      // read sensor reading everytime this loop has been called - every 100 milliseconds
      tiDevice.readXYZData();
      float T = tiDevice.readTemperatureData();
      Serial.println("The temperature data is: " + String(T));
    }
  }
  // put the device to sleep & start a timer of 40 seconds. Once done, wake up the device and start measuring again!
  tiDevice.DEVICE_CONFIG_2_REGFIELD = 0b00000000; // send the device to standby mode
  tiDevice.setOperatingMode("DC2");
  tiDevice.DEVICE_CONFIG_2_REGFIELD = 0b00000001; // send the device to sleep mode
  tiDevice.setOperatingMode("DC2");
  time_now_overall = millis();
  while (millis() < time_now_overall + WAIT_PERIOD_POST_READ)
  {
    // wait for 40 seconds
  }
  // once 40 seconds are complete, wake the device up and start measuring and repeat!
}
