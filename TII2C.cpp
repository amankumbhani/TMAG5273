#include "TII2C.h"
#include "includes\registerMap.h"
#include <Arduino.h>
#include <Wire.h>

#define VERSION 1.0.2

/**
   @brief A constructor to initialise the values to default
*/
TII2C::TII2C(int8_t deviceAddress) {
  this->deviceAddress = deviceAddress;
  DEVICE_CONFIG_1_REGFIELD = 0x00;      /// set to default 
  DEVICE_CONFIG_2_REGFIELD = 0x00;      /// set to default
  SENSOR_CONFIG_1_REGFIELD = 0x74;      /// 0b01110100 - First 4 bits for enabling X, Y & Z magnetic channels & next 4 bits (to LSB) setting sleep time to 20mS.
  SENSOR_CONFIG_2_REGFIELD = 0x04;      /// 0b00000100 - X, Y and Z range set to default (40mT), angle channel X & Y enabled.
  X_THR_CONFIG_REGFIELD = 0x00;         /// set to default
  Y_THR_CONFIG_REGFIELD = 0x00;         /// set to default
  Z_THR_CONFIG_REGFIELD = 0x00;         /// set to default
  T_CONFIG_REGFIELD = 0x01;             /// 0b00000001 - Enabled data acquisition of temperature channel
  INT_CONFIG_1_REGFIELD = 0x00;         /// set to default
  MAG_GAIN_CONFIG_REGFIELD = 0x00;      /// set to default
  MAG_OFFSET_CONFIG_1_REGFIELD = 0x00;  /// set to default
  MAG_OFFSET_CONFIG_2_REGFIELD = 0x00;  /// set to default
  configureDevice();
}

/**
    @brief A function called to initialise all the register field values to their respective states. In order to change the values
    of any register field, use the . operator on the object created in your sketch to access the public member variables to set them,
    and call the function below.
*/
void TII2C::configureDevice() {
  IIC_Write(deviceAddress, DEVICE_CONFIG_1, DEVICE_CONFIG_1_REGFIELD);
  IIC_Write(deviceAddress, DEVICE_CONFIG_2, DEVICE_CONFIG_2_REGFIELD);
  IIC_Write(deviceAddress, SENSOR_CONFIG_1, SENSOR_CONFIG_1_REGFIELD);
  IIC_Write(deviceAddress, SENSOR_CONFIG_2, SENSOR_CONFIG_2_REGFIELD);
  IIC_Write(deviceAddress, X_THR_CONFIG, X_THR_CONFIG_REGFIELD);
  IIC_Write(deviceAddress, Y_THR_CONFIG, Y_THR_CONFIG_REGFIELD);
  IIC_Write(deviceAddress, Z_THR_CONFIG, Z_THR_CONFIG_REGFIELD);
  IIC_Write(deviceAddress, T_CONFIG, T_CONFIG_REGFIELD);
  IIC_Write(deviceAddress, INT_CONFIG_1, INT_CONFIG_1_REGFIELD);
  IIC_Write(deviceAddress, MAG_GAIN_CONFIG, MAG_GAIN_CONFIG_REGFIELD);
  IIC_Write(deviceAddress, MAG_OFFSET_CONFIG_1, MAG_OFFSET_CONFIG_1_REGFIELD);
  IIC_Write(deviceAddress, MAG_OFFSET_CONFIG_2, MAG_OFFSET_CONFIG_2_REGFIELD);
}

/**
    @brief A function called to read configuration settings from the chip. It can be used to read the device ID, manufacturing ID,
    CONV_STATUS register and the DEVICE_STATUS register.
    @param[in] regName: A string indicating which register to be read (abbrevated form).
*/
void TII2C::readConfigurations(String regName) {
  if (regName.equals("DID")) { // read device ID
    byte receivedData = IIC_Read(deviceAddress, DEVICE_ID);
    receivedData = receivedData & 3;
    Serial.print("Device Version is: ");
    Serial.println(receivedData, BIN);
  } else if (regName.equals("MFID")) { // read manufacturing ID
    byte receivedDataLSB = IIC_Read(deviceAddress, MANUFACTURER_ID_LSB);
    byte receivedDataMSB = IIC_Read(deviceAddress, MANUFACTURER_ID_MSB);
    int16_t MFID = ((receivedDataMSB << 8) | receivedDataLSB); // 16 bits that make up the manufacturing data
    Serial.print("Manufacturer ID: ");
    Serial.println(MFID, BIN);
  } else if (regName.equals("CSREG")) { // conv_status register
    byte receivedData = IIC_Read(deviceAddress, CONV_STATUS);
    Serial.print("CONV_STATUS: ");
    Serial.println(receivedData, BIN);
  } else if (regName.equals("DSR")) { // device status register
    byte receivedData = IIC_Read(deviceAddress, DEVICE_STATUS);
    Serial.print("DEVICE_STATUS: ");
    Serial.println(receivedData, BIN);
  }
}

/**
    @brief A function used to set the operating modes ONLY of the chip. 
    @param[in] operatingMode: A string indicating the register that needs to be written (abbrevated form).

*/
void TII2C::setOperatingMode(String operatingMode) {
  if (operatingMode.equals("DC2")) {
    // set the operating mode for device configuration 2 register
    IIC_Write(deviceAddress, DEVICE_CONFIG_2, DEVICE_CONFIG_2_REGFIELD);
  } else if (operatingMode.equals("DC1")) {
    // set the operating mode for device configuration 1 register
    IIC_Write(deviceAddress, DEVICE_CONFIG_1, DEVICE_CONFIG_2_REGFIELD);
  } else if (operatingMode.equals("SC2")) {
    IIC_Write(deviceAddress, SENSOR_CONFIG_2, SENSOR_CONFIG_2_REGFIELD);
    // set parameters of SENSOR_CONFIG_2
  } else if (operatingMode.equals("SC1")) {
    // set parameters of SENSOR_CONFIG_1
    IIC_Write(deviceAddress, SENSOR_CONFIG_1, SENSOR_CONFIG_1_REGFIELD);
  }
}

/**
    @brief A function that reads the angle data from the chip.
    @return angle: returns the angle in degrees.
*/
float TII2C::readAngleData() {
  Wire.beginTransmission(deviceAddress); // transmit to device address
  Wire.write(ANGLE_RESULT_MSB); // sends register address
  Wire.endTransmission(1); // stop transmitting
  Wire.requestFrom(X_MSB_RESULT, 2); // Ask for 2 bytes, once done, bus is released by default
  byte MSB = Wire.read();
  byte LSB = Wire.read();
  int16_t ANGLE_RESULT = ((MSB << 8) | LSB); // 16 bits that make up the X channel data

  float partA, partB;
  int n = ANGLE_RESULT;
  int k, mask, masked_n, thebit = 0;

  for (int i = 4; i < 13; i++) {
    k = i;
    mask = i << k;
    masked_n = n & mask;
    thebit = masked_n >> k;
    partA += (thebit * pow(2, i - 4));
  }
  for (int i = 0; i < 4; i++) {
    k = i;
    mask = i << k;
    masked_n = n & mask;
    thebit = masked_n >> k;
    partB += (thebit * pow(2, i)) / 16;
  }
  float angle = partA + partB;
  return angle;
}

/**
    @brief A function used to read the magnitude data.
*/
float TII2C::readMagnitudeData() {
  Wire.beginTransmission(deviceAddress); // transmit to device address
  Wire.write(MAGNITUDE_RESULT); // sends register address
  Wire.endTransmission(1); // stop transmitting
  Wire.requestFrom(deviceAddress, 1); // Ask for 2 bytes, once done, bus is released by default
  byte result = Wire.read();
  return binaryToDecimal(result);
}

/**
    @brief A function to read the X, Y & Z magnetic channel data and calculate the value in mT.
*/
void TII2C::readXYZData() {
  Wire.beginTransmission(deviceAddress); // transmit to device address
  Wire.write(X_MSB_RESULT); // sends register address
  Wire.endTransmission(1); // stop transmitting
  Wire.requestFrom(deviceAddress, 7); // Ask for 7 bytes, once done, bus is released by default
  byte X_MSB = Wire.read();
  byte X_LSB = Wire.read();
  byte Y_MSB = Wire.read();
  byte Y_LSB = Wire.read();
  byte Z_MSB = Wire.read();
  byte Z_LSB = Wire.read();
  byte DEV_CONF = Wire.read();
  int16_t X_RESULT = ((X_MSB << 8) | X_LSB); // 16bits that make up the X channel data
  int16_t Y_RESULT = ((Y_MSB << 8) | Y_LSB); // 16bits that make up the Y channel data
  int16_t Z_RESULT = ((Z_MSB << 8) | Z_LSB); // 16bits that make up the Z channel data
  float BX, BY, BZ; // magnetic field in mT
  float partA = 0.00;
  float partB = 0.00;

  int n = X_RESULT;
  int k = 15;
  int mask = 1 << k;
  int masked_n = n & mask;
  int thebit = masked_n >> k;

  partA = -(thebit * pow(2, 15)) / (double)pow(2, 16);
  for (int i = 0; i < 15; i++) {
    k = i;
    mask = i << k;
    masked_n = n & mask;
    thebit = masked_n >> k;
    partB += (thebit * pow(2, i)) / (double) pow(2, 16);
  }
  BX = (partA + partB) * (2 * 40);
  Serial.println("X Axis Magnetic Field in mT: " + String(BX));

  partA = 0.00;
  partB = 0.00;
  n = Y_RESULT;
  k = 15;
  mask = 1 << k;
  masked_n = n & mask;
  thebit = masked_n >> k;
  partA = -(thebit * pow(2, 15)) / (double)pow(2, 16);
  for (int i = 0; i < 15; i++) {
    k = i;
    mask = i << k;
    masked_n = n & mask;
    thebit = masked_n >> k;
    partB += (thebit * pow(2, i)) / (double) pow(2, 16);
  }
  BY = (partA + partB) * (2 * 40);
  Serial.println("Y Axis Magnetic Field in mT: " + String(BY));

  partA = 0.00;
  partB = 0.00;
  n = Z_RESULT;
  k = 15;
  mask = 1 << k;
  masked_n = n & mask;
  thebit = masked_n >> k;
  partA = -(thebit * pow(2, 15)) / (double)pow(2, 16);
  for (int i = 0; i < 15; i++) {
    k = i;
    mask = 1 << k;
    masked_n = n & mask;
    thebit = masked_n >> k;
    partB += (thebit * pow(2, i)) / (double) pow(2, 16);
  }
  BZ = (partA + partB) * (2 * 40);
  Serial.println("Z Axis Magnetic Field in mT: " + String(BZ));
}

/**
    @brief A function used to read the temperature from the chip. 
    @return T: Temperature in degrees celcius.
*/
float TII2C::readTemperatureData() {
  Wire.beginTransmission(deviceAddress); // transmit to device address
  Wire.write(T_MSB_RESULT); // sends register address
  Wire.endTransmission(1); // stop transmitting
  Wire.requestFrom(deviceAddress, 2); // Ask for 2 bytes, once done, bus is released by default
  byte LSB = Wire.read();
  byte MSB = Wire.read();
  int16_t temperatureData = ((MSB << 8) | LSB); //16bits that make up the temperature data
  int TADCT = binaryToDecimal(temperatureData);
  int TSENSET0 = 25;
  float TADCRES = 60.1;
  float TADCT0 = 17508;
  float T = TSENSET0 + ((TADCT - TADCT0) / (TADCRES));
  return T;
}

/**
    @brief A function that returns a decimal number given a binary number.
*/
int TII2C::binaryToDecimal(int16_t n)
{
  int num = n;
  int dec_value = 0;

  // Initializing base value to 1, i.e 2^0
  int base = 1;

  int temp = num;
  while (temp) {
    int last_digit = temp % 10;
    temp = temp / 10;

    dec_value += last_digit * base;

    base = base * 2;
  }

  return dec_value;
}

void TII2C::hexToBinaryHelper(uint8_t a) {
  uint8_t i;
  for (i = 0x80; i != 0; i >>= 1) Serial.printf("%c", (a & i) ? '1' : '0');
  Serial.println();
}

void TII2C::IIC_Write(int8_t deviceAddress, int8_t registerAddress, int8_t dataToWrite) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(registerAddress);
  Wire.write(dataToWrite);
  Wire.endTransmission(1);
}

byte TII2C::IIC_Read(int8_t deviceAddress, int8_t registerAddress) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(registerAddress);
  Wire.endTransmission(1);
  Wire.requestFrom(deviceAddress, 1);
  while (!Wire.available()); // wait for the data to come back
  return Wire.read();
}
