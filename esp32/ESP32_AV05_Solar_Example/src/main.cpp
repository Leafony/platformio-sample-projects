/**
 * @file main.cpp
 * @brief
 * AV05ソーラーチャージャーリーフの電源処理を行うサンプルスケッチです。
 * バッテリーを接続し、電源ボタンを押下すると3秒間LEDが点灯し、その後消灯します。（電源ON処理）
 * 電源供給中に電源ボタンを押下し続けると、LEDが0.5秒間隔で点滅し、10秒後に電源が切れます。（電源OFF処理）
 */
#include <Arduino.h>
#include <Wire.h>
#include <ADC101C.h>
#include <TCA9536.h>
#include "main.h"

/*********************************************************************
 * グローバルインスタンス
 *********************************************************************/
ADC101C adc(0x50);
TCA9536 io;

/*********************************************************************
 * グローバル変数
 *********************************************************************/
#if defined(ESP32)
hw_timer_t *timer0 = NULL; // 電源投入後のLED消灯用タイマー
hw_timer_t *timer1 = NULL; // 電源ボタン押下時のLED点滅用タイマー
#else
HardwareTimer *timer0 = NULL; // 電源投入後のLED消灯用タイマー
HardwareTimer *timer1 = NULL; // 電源ボタン押下時のLED点滅用タイマー
#endif

volatile bool b_psw_pushed = false; // 電源ボタンが押されたフラグ

volatile bool b_timer0_timeout = false;     // タイマー0タイムアウトフラグ
volatile uint32_t count_timer0_timeout = 0; // タイマー0タイムアウトカウンタ

volatile bool b_timer1_timeout = false; // タイマー1タイムアウトフラグ

#if defined(ESP32)
/**
 * @brief 電源ボタンが押されたときの割り込み処理
 *
 */
void IRAM_ATTR onPowerButtonPushed()
{
  b_psw_pushed = true;
}

/**
 * @brief 電源ボタンLED用タイマー割り込み処理（電源ON処理用）
 *
 */
void IRAM_ATTR onTimer0()
{
  b_timer0_timeout = true;
}

/**
 * @brief 電源ボタンLED用タイマー割り込み処理（電源OFF処理用）
 *
 */
void IRAM_ATTR onTimer1()
{
  b_timer1_timeout = true;
}
#else
/**
 * @brief 電源ボタンが押されたときの割り込み処理
 *
 */
void onPowerButtonPushed()
{
  b_psw_pushed = true;
}

/**
 * @brief 電源ボタンLED用タイマー割り込み処理（電源ON処理用）
 *
 */
void onTimer0()
{
  b_timer0_timeout = true;
}

/**
 * @brief 電源ボタンLED用タイマー割り込み処理（電源OFF処理用）
 *
 */
void onTimer1()
{
  b_timer1_timeout = true;
}
#endif // defined(ESP32)

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
 * @brief
 *
 */
void checkBatteryStatus()
{
  LT3652_CHARGE_STATUS_t chargeStatus;

  // 充電ステータスを取得
  chargeStatus = getChargeStatus();
  if (chargeStatus == LT3652_CHARGE_STATUS_STANDBY)
  {
    Serial.print("充電OFF: ");
  }
  else if (chargeStatus == LT3652_CHARGE_STATUS_CHARGING)
  {
    Serial.print("充電中: ");
  }
  else
  {
    Serial.print("充電異常: ");
  }

  // 電池電圧を読取り
  float dataBatt = readBatteryVoltage();
  Serial.print("Batt[V]  = " + String(dataBatt));
  Serial.print(", ");

  int pws = digitalRead(INT_PSW_N_PIN);
  Serial.println("PWS = " + String(pws));
}

/**
 * @brief
 *
 */
void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    ;
  }

  Wire.begin();

  // ADCを初期化
  if (!adc.begin())
  {
    Serial.println("Error: ADC101C is not connected! Freezing...");
    return;
  }

  // IOエキスパンダーを初期化
  if (initIoExpander() == false)
  {
    Serial.println("Failed to initialize IO expander. Freezing...");
    return;
  }

  // 電源ボタン割り込みを有効化
  pinMode(INT_PSW_N_PIN, INPUT);
  attachInterrupt(INT_PSW_N_PIN, onPowerButtonPushed, RISING);

  turnOnLed(); // 電源ボタンLEDを点灯

#if defined(ESP32)
  // 電源ボタンを一定期間後に消灯させるタイマーを設定
  timer0 = timerBegin(0, 80, true);                  // 80サイクルで1カウント = 1us
  timerAttachInterrupt(timer0, &onTimer0, true);     // 割り込み関数を登録
  timerAlarmWrite(timer0, 0.5 * 1000 * 1000, false); // 0.5秒間隔で割り込み
  timerAlarmEnable(timer0);                          // カウント開始

  // 電源ボタンを一定間隔で点滅させるタイマーを設定
  timer1 = timerBegin(1, 80, true);                  // 80サイクルで1カウント = 1us
  timerAttachInterrupt(timer1, &onTimer1, true);     // 割り込み関数を登録
  timerAlarmWrite(timer1, 0.5 * 1000 * 1000, false); // 0.5秒間隔で割り込み開始
#else
  // 電源ボタンを一定期間後に消灯させるタイマーを設定
  timer0 = new HardwareTimer(TIM1);
  timer0->setMode(1, TIMER_OUTPUT_COMPARE);
  timer0->setOverflow(2, HERTZ_FORMAT); // 0.5秒間隔で割り込み
  timer0->attachInterrupt(onTimer0);    // 割り込み関数を登録
  timer0->resume();                     // カウント開始

  // 電源ボタンを一定間隔で点滅させるタイマーを設定
  timer1 = new HardwareTimer(TIM2);
  timer1->setMode(1, TIMER_OUTPUT_COMPARE);
  timer1->setOverflow(2, HERTZ_FORMAT);
  timer1->attachInterrupt(onTimer1);
#endif // defined(ESP32)

  enable5VSupply(); // 5V出力をON

  delay(10);
}

void loop()
{

  /**
   * 電源ON時のタイマー割り込み処理
   */
  if (b_timer0_timeout == true) // 0.5秒でタイムアウト
  {
    b_timer0_timeout = false;
    count_timer0_timeout++;

    // 電源投入後、一定期間経過後、電源ボタンLEDを消灯
    if (count_timer0_timeout >= 3 * 2 /* 3秒 * 2Hz */)
    {
      // 電源ボタンLEDを消灯
      io.write(IOEX_O_LED_N_PIN, PSW_LED_OFF);
      count_timer0_timeout = 0;

      // タイマーを停止
#if defined(ESP32)
      timerAlarmDisable(timer0);
#else
      timer0->pause();
#endif // defined(ESP32)
    }

#if defined(ESP32)
    timerWrite(timer0, 0); // タイマーをリセット
    timerAlarmEnable(timer0);
#else
    // NOP
#endif // defined(ESP32)
  }

  /**
   * 電源ボタンが押された時の処理
   */
  if (b_psw_pushed == true)
  {
    b_psw_pushed = false;
    Serial.println("Power button pushed!");
    delay(100); // チャタリング対策で一定時間待つ

    // LED点滅用タイマーを有効化
#if defined(ESP32)
    timerAlarmEnable(timer1);
#else
    timer1->resume();
#endif // defined(ESP32)
  }

  /**
   * 電源OFF時のタイマー割り込み処理
   */
  if (b_timer1_timeout == true) // 0.5秒でタイムアウト
  {
    b_timer1_timeout = false;
    /**
     * スイッチが押されている時だけLEDを点滅
     */
    if (digitalRead(INT_PSW_N_PIN) == PSW_PUSHED)
    {
      // 電源ボタンが押されている間はLEDを点滅
      toggleLed();
    }
    else
    {
      // 電源ボタンが離されたらLEDを消灯
      turnOffLed();
    }

#if defined(ESP32)
    timerWrite(timer1, 0); // タイマーをリセット
    timerAlarmEnable(timer1);
#else
    // NOP
#endif // defined(ESP32)
  }

  // バッテリーリーフの状態をシリアルに出力
  checkBatteryStatus();

  /**
   * 以下、任意の処理を追加してください
   */
}
