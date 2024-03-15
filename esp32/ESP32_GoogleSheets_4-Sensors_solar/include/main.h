#ifndef __MAIN_H__
#define __MAIN_H__

#define PSW_PUSHED LOW
#define PSW_RELEASED HIGH
#define INT_PSW_N_PIN 27

/**
 * プロトタイプ
 */
void onPowerButtonPushed();
void onTimer0();
void onTimer1();
float checkBatteryStatus();
void connectWiFi();
void accessToGoogleSheets(float temperature, float humidity, float illumination, float dataBatt);
void espLightSleep(int period);
void espDeepSleep(int period);
void mainTask();

#endif // __MAIN_H__
