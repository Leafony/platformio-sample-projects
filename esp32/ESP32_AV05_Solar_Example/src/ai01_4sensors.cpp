#include "ai01_4sensors.h"

// instance to handle a bright sensor
ClosedCube_OPT3001 illum;

/**
 * @brief
 *
 * @return true
 * @return false
 */
void initSensors()
{
  OPT3001_Config illumConfig;
  OPT3001_ErrorCode illumErrorConfig;

  // I2C for bright sensor is initialized
  // intialize i2c communication
  Wire.begin();
  illum.begin(OPT3001_ADDRESS);

  // I2C for temperature and humidity sensor is initialized
  // Data pin is initialized exceptionally for ESP32 Leaf's I2C Bugs
  pinMode(21, OUTPUT);
  digitalWrite(21, 0);
  Wire.begin();
  smeHumidity.begin();

  illumConfig.RangeNumber = B1100;             // automatic full scale
  illumConfig.ConvertionTime = B1;             // convertion time = 800ms
  illumConfig.ModeOfConversionOperation = B11; // continous conversion
  illumConfig.Latch = B0;                      // hysteresis-style
  illumErrorConfig = illum.writeConfig(illumConfig);
  if (illumErrorConfig != NO_ERROR)
  {
    illumErrorConfig = illum.writeConfig(illumConfig); // retry
  }
}

/**
 * @brief
 *
 * @return float
 */
float readTemperature()
{
  // sensor calibration data
  float TL0 = 25.0;
  float TM0 = 25.0;
  float TL1 = 40.0;
  float TM1 = 40.0;

  // read temperature
  float temperature = (float)smeHumidity.readTemperature();
  // calibrate temperature
  temperature = TM0 + (TM1 - TM0) * (temperature - TL0) / (TL1 - TL0);
  return temperature;
}

/**
 * @brief
 *
 * @return float
 */
float readHumidity()
{
  // sensor calibration data
  float HL0 = 60.0;
  float HM0 = 60.0;
  float HL1 = 80.0;
  float HM1 = 80.0;

  // read temperature
  float humidity = (float)smeHumidity.readHumidity();
  // calibrate temperature
  humidity = HM0 + (HM1 - HM0) * (humidity - HL0) / (HL1 - HL0);
  return humidity;
}

/**
 * @brief
 *
 * @return float
 */
float readIllumination()
{
  OPT3001 result = illum.readResult();
  float illumination;

  if (result.error == NO_ERROR)
  {
    illumination = result.lux;
  }
  else
  {
    illumination = -99.9;
  }
  return illumination;
}
