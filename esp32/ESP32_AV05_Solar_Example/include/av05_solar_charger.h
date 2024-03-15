#ifndef __AV05_SOLAR_CHARGER_H__
#define __AV05_SOLAR_CHARGER_H__

#include <Arduino.h>
#include <Wire.h>
#include <TCA9536.h>
#include <ADC101C.h>

/**
 * IOエキスパンダーピンアサイン
 */
#define IOEX_I_CHRG_N_PIN 0
#define IOEX_I_FAULT_N_PIN 1
#define IOEX_O_ON5V_PIN 2
#define IOEX_O_LED_N_PIN 3

#define PSW_LED_ON LOW
#define PSW_LED_OFF HIGH

/**
 * LT3652充電ステータス
 */
typedef enum
{
  LT3652_CHARGE_STATUS_OVERHEAT = -3,
  LT3652_CHARGE_STATUS_BATTERY_ERROR = -2,
  LT3652_CHARGE_STATUS_UNKNOWN = -1,
  LT3652_CHARGE_STATUS_STANDBY = 1,
  LT3652_CHARGE_STATUS_CHARGING = 2,
} LT3652_CHARGE_STATUS_t;

/**
 * プロトタイプ
 */
bool initAv05SolarCharger();
bool initIoExpander();
void turnOnLed();
void turnOffLed();
void toggleLed();
void enable5VSupply();
void disable5VSupply();
float readBatteryVoltage();
LT3652_CHARGE_STATUS_t getChargeStatus();

#endif // __AP05_SOLAR_CHARGER_H__
