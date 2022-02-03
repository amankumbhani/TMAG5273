/**
   @author Aman Kumbhani
*/
#include "TII2C.h"
#include <Wire.h>

#define WAIT_PERIOD_POST_READ 40000
#define SAMPLING_PERIOD 100
#define READ_PERIOD 20000
#define deviceAddress 0x35

TII2C tiDevice(deviceAddress);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
}

void loop() {
  float time_now_overall = millis();
  unsigned long startMillis = millis();
  unsigned long currentMillis;
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
