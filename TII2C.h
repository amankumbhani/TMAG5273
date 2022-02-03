#ifndef TII2C_H
#define TII2C_H

#include <stdint.h>
#include <Arduino.h>

class TII2C {
  public:
    int8_t deviceAddress;
    int8_t DEVICE_CONFIG_1_REGFIELD;
    int8_t DEVICE_CONFIG_2_REGFIELD;
    int8_t SENSOR_CONFIG_1_REGFIELD;
    int8_t SENSOR_CONFIG_2_REGFIELD;
    int8_t X_THR_CONFIG_REGFIELD;
    int8_t Y_THR_CONFIG_REGFIELD;
    int8_t Z_THR_CONFIG_REGFIELD;
    int8_t T_CONFIG_REGFIELD;
    int8_t INT_CONFIG_1_REGFIELD;
    int8_t MAG_GAIN_CONFIG_REGFIELD;
    int8_t MAG_OFFSET_CONFIG_1_REGFIELD;
    int8_t MAG_OFFSET_CONFIG_2_REGFIELD;
    int8_t I2C_ADDRESS_REGFIELD;
    int8_t DEVICE_ID_REGFIELD;
    int8_t MANUFACTURER_ID_REGFIELD;
    int8_t MANUFACTURER_ID_MSB_REGFIELD;
    int8_t T_MSB_RESULT_REGFIELD;
    int8_t T_LSB_RESULT_REGFIELD;
    int8_t X_LSB_RESULT_REGFIELD;
    int8_t X_MSB_RESULT_REGFIELD;
    int8_t Y_MSB_RESULT_REGFIELD;
    int8_t Y_LSB_RESULT_REGFIELD;
    int8_t Z_MSB_RESULT_REGFIELD;
    int8_t Z_LSB_RESULT_REGFIELD;
    int8_t CONV_STATUS_REGFIELD;
    int8_t ANGLE_RESULT_MSB_REGFIELD;
    int8_t ANGLE_RESULT_LSB_REGFIELD;
    int8_t MAGNITUDE_RESULT_REGFIELD;
    int8_t DEVICE_STATUS_REGFIELD;

    TII2C(int8_t deviceAddress = 0x35);

    void configureDevice();
    void setOperatingMode(String operatingMode);
    float readTemperatureData();
    void readConfigurations(String regName);
    float readAngleData();
    void readXYZData();
    void readMagnitudeData();
    void IIC_Write(int8_t deviceAddress, int8_t registerAddress, int8_t dataToWrite);
    byte IIC_Read(int8_t deviceAddress, int8_t registerAddress);
    void hexToBinaryHelper(uint8_t a);
    int binaryToDecimal(int16_t n);
};

#endif
