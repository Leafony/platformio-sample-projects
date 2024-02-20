#ifndef ADC101C_H
#define ADC101C_H

#include <Wire.h>

typedef enum
{
  ADC101C_REGISTER_RESULT = 0x00,
  ADC101C_REGISTER_ALERT,
  ADC101C_REGISTER_CONFIG,
  ADC101C_REGISTER_LIMITL,
  ADC101C_REGISTER_LIMITH,
  ADC101C_REGISTER_HYST,
  ADC101C_REGISTER_CONVL,
  ADC101C_REGISTER_CONVH
} ADC101C_REGISTER_t;

typedef enum
{
  ADC101C_ERROR_READ = -4,
  ADC101C_ERROR_WRITE = -3,
  ADC101C_ERROR_INVALID_ADDRESS = -2,
  ADC101C_ERROR_UNDEFINED = -1,
  ADC101C_ERROR_SUCCESS = 1
} ADC101C_error_t;
const ADC101C_error_t ADC101C_SUCCESS = ADC101C_ERROR_SUCCESS;

class ADC101C
{
public:
  ADC101C(uint8_t i2cAddr);
  bool begin(TwoWire &wirePort = Wire);
  bool isConnected();

  uint16_t read();

private:
  TwoWire *_i2cPort;
  uint8_t _i2cAddr;
  ADC101C_error_t readI2CBuffer(uint8_t *dest, ADC101C_REGISTER_t reg, uint16_t len);
  ADC101C_error_t writeI2CBuffer(uint8_t *src, ADC101C_REGISTER_t reg, uint16_t len);
  ADC101C_error_t readI2CRegister(uint8_t *dest, ADC101C_REGISTER_t reg);
  ADC101C_error_t writeI2CRegister(uint8_t *src, ADC101C_REGISTER_t reg);
};

#endif
