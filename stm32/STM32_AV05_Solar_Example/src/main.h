#ifndef __MAIN_H__
#define __MAIN_H__

/**
 * IOエキスパンダーピンアサイン
 */
#define IOEX_I_CHRG_N_PIN 0
#define IOEX_I_FAULT_N_PIN 1
#define IOEX_O_ON5V_PIN 2
#define IOEX_O_LED_N_PIN 3

#define PSW_PUSHED LOW
#define PSW_RELEASED HIGH

#define PSW_LED_ON LOW
#define PSW_LED_OFF HIGH

/**
 * 割り込みピン
 */
#if defined(ESP32)
#define INT_PSW_N_PIN 27
#else
#define INT_PSW_N_PIN 3
#endif // ESP32

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
void onPowerButtonPushed();
void onTimer0();
void onTimer1();
float readBatteryVoltage();
LT3652_CHARGE_STATUS_t getChargeStatus();
bool initIoExpander();
void turnOnLed();
void turnOffLed();
void toggleLed();
void enable5VSupply();
void disable5VSupply();
void checkBatteryStatus();

#endif // __MAIN_H__
