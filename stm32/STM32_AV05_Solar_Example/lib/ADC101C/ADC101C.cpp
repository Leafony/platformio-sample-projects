#include <ADC101C.h>

ADC101C::ADC101C(uint8_t i2cAddr)
{
  _i2cPort = NULL;
  _i2cAddr = i2cAddr;
}

bool ADC101C::begin(TwoWire &wirePort)
{
  _i2cPort = &wirePort;
  return (isConnected());
}

bool ADC101C::isConnected(void)
{
  _i2cPort->beginTransmission(_i2cAddr);
  return (_i2cPort->endTransmission() == 0);
}

uint16_t ADC101C::read(void)
{
  ADC101C_error_t err;
  uint8_t data[2];

  err = readI2CBuffer(data, ADC101C_REGISTER_RESULT, 2);
  if (err != ADC101C_SUCCESS)
  {
    return 0;
  }
  return (((uint16_t)(data[1] & 0x0f) << 6) | (data[0] >> 2));
}

ADC101C_error_t ADC101C::readI2CBuffer(uint8_t *dest, ADC101C_REGISTER_t reg, uint16_t len)
{
  uint8_t timeout = 0;
  _i2cPort->beginTransmission(_i2cAddr);
  _i2cPort->write(reg);
  if (_i2cPort->endTransmission(false) != 0)
  {
    return ADC101C_ERROR_READ;
  }
  _i2cPort->requestFrom(_i2cAddr, len);
  for (uint16_t i = 0; i < len; i++)
  {
    dest[i] = _i2cPort->read();
  }
  return ADC101C_SUCCESS;
}

ADC101C_error_t ADC101C::writeI2CBuffer(uint8_t *src, ADC101C_REGISTER_t reg, uint16_t len)
{
  _i2cPort->beginTransmission(_i2cAddr);
  _i2cPort->write(reg);
  for (uint16_t i = 0; i < len; i++)
  {
    _i2cPort->write(src[i]);
  }
  if (_i2cPort->endTransmission() != 0)
  {
    return ADC101C_ERROR_WRITE;
  }
  return ADC101C_SUCCESS;
}

ADC101C_error_t ADC101C::readI2CRegister(uint8_t *dest, ADC101C_REGISTER_t reg)
{
  return readI2CBuffer(dest, reg, 1);
}

ADC101C_error_t ADC101C::writeI2CRegister(uint8_t *src, ADC101C_REGISTER_t reg)
{
  return writeI2CBuffer(src, reg, 1);
}
