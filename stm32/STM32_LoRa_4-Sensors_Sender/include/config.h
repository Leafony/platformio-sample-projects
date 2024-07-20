#ifndef __CONFIG_H__
#define __CONFIG_H__

// Set to true to enable debugging output
#define DEBUG true
#define DEBUG_SERIAL \
  if (DEBUG)         \
  Serial

// システム起動間隔 (ms)
#define WAKEUP_INTERVAL_MS 1 * 60 * 1000 // 1分

// ランダム送信間隔 (ms)
// LoRaの輻輳を避けるために付与するランダムな送信オフセット時間の最大値
#define RANDOM_INTERVAL_MS 1 * 1000 // 1秒

// IOエキスパンダーのピン番号
#define IOEX_PIN_0 0
#define IOEX_PIN_1 1
#define IOEX_PIN_2 2
#define IOEX_PIN_3 3

// LoRaのピン番号
#define LORA_BOOT0 IOEX_PIN_0
#define LORA_RESET IOEX_PIN_2
#define LORA_IRQ_DUMB D10

// LoRa周波数
#define LORA_FREQUENCY 923E6 // AS923
// LoRa送信電力
#define LORA_TX_POWER 17 // dBm

// I2Cアドレス
#define BATT_ADC_ADDR 0x50

#endif // __CONFIG_H__
