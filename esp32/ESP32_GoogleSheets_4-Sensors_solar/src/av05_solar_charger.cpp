#include <main.h>
#include <av05_solar_charger.h>

ADC101C adc(0x50);
TCA9536 io;

/**
 * @brief AV05ソーラーチャージャーを初期化
 *
 */
bool initAv05SolarCharger()
{
  // ADCを初期化
  if (!adc.begin())
  {
    Serial.println("Error: ADC101C is not connected! Freezing...");
    return false;
  }

  // IOエキスパンダーを初期化
  if (initIoExpander() == false)
  {
    Serial.println("Failed to initialize IO expander. Freezing...");
    return false;
  }

  return true;
}

/**
 * @brief IOエキスパンダーを初期化
 *
 */
bool initIoExpander()
{
  if (io.begin() == false)
  {
    Serial.println("Error: TCA9536 is not detected!");
    return false;
  }

  // IOエキスパンダーを初期化
  io.pinMode(IOEX_I_CHRG_N_PIN, INPUT);
  io.pinMode(IOEX_I_FAULT_N_PIN, INPUT);

  io.write(IOEX_O_ON5V_PIN, LOW);
  io.pinMode(IOEX_O_ON5V_PIN, OUTPUT);

  io.write(IOEX_O_LED_N_PIN, PSW_LED_OFF);
  io.pinMode(IOEX_O_LED_N_PIN, OUTPUT);
  return true;
}

/**
 * @brief 電源ボタンLEDを点灯
 *
 */
void turnOnLed()
{
  io.write(IOEX_O_LED_N_PIN, PSW_LED_ON);
}

/**
 * @brief 電源ボタンLEDを消灯
 *
 */
void turnOffLed()
{
  io.write(IOEX_O_LED_N_PIN, PSW_LED_OFF);
}

/**
 * @brief 電源ボタンLEDをトグル
 *
 */
void toggleLed()
{
  io.toggle(IOEX_O_LED_N_PIN);
}

/**
 * @brief 5V出力をON
 *
 */
void enable5VSupply()
{
  io.write(IOEX_O_ON5V_PIN, HIGH);
}

/**
 * @brief 5V出力をOFF
 *
 */
void disable5VSupply()
{
  io.write(IOEX_O_ON5V_PIN, LOW);
}

/**
 * @brief バッテリー電圧読み取り
 *
 * @return float バッテリー電圧値
 */
float readBatteryVoltage()
{
  uint16_t adcVal = adc.read();
  float dataBatt = (float)(adcVal * 3.3 / 1024 * 2);
  delay(100);
  return dataBatt;
}

/**
 * @brief 充電ステータス読み取り
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
  else
  {
    return LT3652_CHARGE_STATUS_UNKNOWN;
  }
}
