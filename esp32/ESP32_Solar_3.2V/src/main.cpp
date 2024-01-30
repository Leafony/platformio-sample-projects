#include <Arduino.h>
#include <Wire.h>
#include <ADC101C.h>
#include <TCA9536.h>

/**
 * IOエキスパンダーピンアサイン
 */
#define IOEX_I_CHRG_N_PIN 0
#define IOEX_I_FAULT_N_PIN 1
#define IOEX_O_ON5V_PIN 2
#define IOEX_O_LED_N_PIN 3

/**
 * 割り込みピン
 */
#define INT_PSW_N_PIN 3

/**
 * LT3652充電ステータス
 */
typedef enum
{
  LT3652_CHARGE_STATUS_OVERHEAT = -2,
  LT3652_CHARGE_STATUS_BATTERY_ERROR = -1,
  LT3652_CHARGE_STATUS_STANDBY = 1,
  LT3652_CHARGE_STATUS_CHARGING = 2,
} LT3652_CHARGE_STATUS_t;

/**
 * グローバルインスタンス
 */
ADC101C adc(0x50);
TCA9536 io;

/**
 * @brief 電源ボタンが押されたときの処理
 *
 */
void onPushPowerButton()
{
  Serial.println("pushed!");
}

/**
 * @brief Read battery voltage
 *
 * @return float
 */
float readBatteryVoltage()
{
  uint16_t adcVal = adc.read();
  float dataBatt = (float)(adcVal * 3.3 / 1024 * 2);
  return dataBatt;
}

/**
 * @brief 充電ステータスを読取り
 *
 * @return LT3652_CHARGE_STATUS_t
 */
LT3652_CHARGE_STATUS_t getChargeStatus()
{
  uint8_t chrg = io.read(IOEX_I_CHRG_N_PIN);
  uint8_t fault = io.read(IOEX_I_FAULT_N_PIN);
  if (chrg == 0x01 && fault == 0x01)
  {
    return LT3652_CHARGE_STATUS_STANDBY;
  }
  else if (chrg == 0x00 && fault == 0x01)
  {
    return LT3652_CHARGE_STATUS_BATTERY_ERROR;
  }
  else if (chrg == 0x01 && fault == 0x00)
  {
    return LT3652_CHARGE_STATUS_CHARGING;
  }
  else if (chrg == 0x00 && fault == 0x00)
  {
    return LT3652_CHARGE_STATUS_OVERHEAT;
  }
}

/**
 * @brief
 *
 */
void setup()
{
  Serial.begin(115200);
  // while (!Serial)
  // {
  //   ;
  // }

  Wire.begin();

  if (!adc.begin())
  {
    Serial.println("ADC101C is not connected!");
    return;
  }

  if (io.begin() == false)
  {
    Serial.println("TCA9536 is not detected. Please check wiring. Freezing...");
    return;
  }

  // IOエキスパンダーを初期化
  io.pinMode(IOEX_I_CHRG_N_PIN, INPUT);
  io.pinMode(IOEX_I_FAULT_N_PIN, INPUT);
  io.pinMode(IOEX_O_ON5V_PIN, OUTPUT);
  io.pinMode(IOEX_O_LED_N_PIN, OUTPUT);

  // 電源ボタンLEDを点灯
  io.write(IOEX_O_LED_N_PIN, LOW);

  // 5V出力をON
  io.write(IOEX_O_ON5V_PIN, HIGH);

  // 電源ボタン割り込みを有効化
  attachInterrupt(INT_PSW_N_PIN, onPushPowerButton, RISING);

  delay(10);
}

void loop()
{
  LT3652_CHARGE_STATUS_t chargeStatus;

  // 電池電圧を読取り
  float dataBatt = readBatteryVoltage();
  Serial.println("Batt[V]  = " + String(dataBatt));

  // 充電ステータスを取得
  chargeStatus = getChargeStatus();
  if (chargeStatus == LT3652_CHARGE_STATUS_STANDBY)
  {
    Serial.println("充電OFF");
  }
  else if (chargeStatus == LT3652_CHARGE_STATUS_CHARGING)
  {
    Serial.println("充電中");
  }
  else
  {
    Serial.println("充電異常");
  }
  delay(2000);
}
