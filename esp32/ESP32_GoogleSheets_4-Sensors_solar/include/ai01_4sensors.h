#ifndef __AI01_4SENSORS_H__
#define __AI01_4SENSORS_H__

#include <Arduino.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <HTS221.h>
#include <ClosedCube_OPT3001.h>

#define OPT3001_ADDRESS 0x45 // ADDR pin = VCC

void initSensors();
float readTemperature();
float readHumidity();
float readIllumination();

#endif // __AI01_4SENSORS_H__
