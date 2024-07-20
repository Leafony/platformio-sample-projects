#include <Arduino.h>
#include <IWatchdog.h>
#include <STM32LowPower.h>
#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <TCA9536.h>
#include <LoRa.h>

#include "config.h"

void SystemClock_Config(void);
void resetLoRa(bool path_through);
String readBatteryVoltage();

TCA9536 io(TCA9536_Address_t::TCA9536A_ADDRESS);

/**
 * @brief LoRa初期化
 *
 * @param path_through パススルーモード有効化
 */
void resetLoRa(bool path_through = false)
{
  // SPIパススルーモード
  pinMode(LORA_IRQ_DUMB, OUTPUT);
  if (path_through)
  {
    digitalWrite(LORA_IRQ_DUMB, LOW);
  }
  else
  {
    digitalWrite(LORA_IRQ_DUMB, HIGH);
  }
  delay(100);

  // ハードウェアリセット
  io.write(LORA_BOOT0, LOW);

  io.write(LORA_RESET, HIGH);
  delay(200);
  io.write(LORA_RESET, LOW);
  delay(200);
  io.write(LORA_RESET, HIGH);
  delay(50);
}

/**
 * @brief バッテリ電圧読み出し
 * @details mV = adcVal * Vref(3.3V) / resolution(8bit) * Vdiv(2)
 * @return 2桁のHEX String
 */
String readBatteryVoltage()
{
  String strAdcVal;
  Wire.beginTransmission(BATT_ADC_ADDR);
  Wire.write(0x00);
  Wire.endTransmission(false);
  Wire.requestFrom(BATT_ADC_ADDR, 2);
  uint8_t adcVal1 = Wire.read();
  uint8_t adcVal2 = Wire.read();
  uint8_t adcVal = (adcVal1 << 4) | (adcVal2 >> 4);
  strAdcVal = String(adcVal, HEX);
  if (adcVal < 16)
    strAdcVal = '0' + strAdcVal;
  strAdcVal.toUpperCase();
  return strAdcVal;
}

/**
 * @brief
 *
 */
void setup()
{
  // Initialize the IWDG with 10 seconds timeout.
  // This would cause a CPU reset if the IWDG timer
  // is not reloaded in approximately 10 seconds.
  IWatchdog.begin(10 * 1000 * 1000);

  // CPUスピードを80MHz→16MHzに変更
  // 低速にすることでLoRaのSPI通信速度を下げることができ、
  // 安定して通信することができる
  SystemClock_Config();

  // STM32LowPowerを初期化
  LowPower.begin();

  // 乱数設定
  randomSeed(analogRead(A0));

#if DEBUG == true
  // シリアルを初期化
  Serial.begin(115200);
  while (!Serial)
    ;
#endif // DEBUG

  DEBUG_SERIAL.println("Starting LoRa 4-Sensors Sender...");

  // I2Cを初期化
  Wire.begin();

  // IOエキスパンダーを初期化
  if (io.begin() == false)
  {
    DEBUG_SERIAL.println("TCA9536 not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }
  io.disablePullUp(true);
  io.pinMode(LORA_RESET, OUTPUT);
  io.pinMode(LORA_BOOT0, OUTPUT);
  io.write(LORA_BOOT0, HIGH);

  // LoRaを初期化
  resetLoRa(false);
  resetLoRa(true);
  if (LoRa.begin(LORA_FREQUENCY) == false)
  {
    DEBUG_SERIAL.println("Starting LoRa failed!");
    while (1)
      ;
  }
  LoRa.enableCrc();               // CRCを有効化
  LoRa.setTxPower(LORA_TX_POWER); // 送信電力を設定

  DEBUG_SERIAL.println("Successfully started!");
}

/**
 * @brief
 *
 */
void loop()
{
  long x, y, z;
  uint16_t temp;
  char xString[9], yString[9], zString[9], tempString[5];
  String batString;

  DEBUG_SERIAL.print("Sending packet... ");

  // 電池電圧を読み出し
  batString = readBatteryVoltage();

  // LoRaでデータを送信
  LoRa.beginPacket();
  LoRa.write(0xFF);
  LoRa.print(batString);
  LoRa.endPacket();

  DEBUG_SERIAL.println("end!");

  // LoRaをスリープ
  LoRa.end();
  digitalWrite(LORA_IRQ_DUMB, LOW);
  io.write(LORA_BOOT0, LOW);
  io.write(LORA_RESET, LOW);

  // ピンを入力にして電流リークを防ぐ
  pinMode(LORA_IRQ_DUMB, INPUT);
  io.pinMode(LORA_RESET, INPUT);
  io.pinMode(LORA_BOOT0, INPUT);

  // ランダムな時間待機を生成し、輻輳を避ける
  int randomInterval = random(RANDOM_INTERVAL_MS);

  // IWDGをリロード
  IWatchdog.reload();

  // システムを一定期間停止
  DEBUG_SERIAL.println("Restart after " + String(WAKEUP_INTERVAL_MS + randomInterval) + " ms...");
  DEBUG_SERIAL.flush();
  LowPower.shutdown(WAKEUP_INTERVAL_MS + randomInterval);
}
