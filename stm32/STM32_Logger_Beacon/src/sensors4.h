#ifndef sensors4_h
#define sensors4_h

#include <HTS221.h>
#include <ClosedCube_OPT3001.h>
#include <Adafruit_LIS3DH.h>

void setupSensors(void);
void readSensors(float* temp, float* humid, float* illum, float* batt);
void wakeupSensors(void);
void sleepSensors(void);
void enableAccelInterrupt(void);

#endif
