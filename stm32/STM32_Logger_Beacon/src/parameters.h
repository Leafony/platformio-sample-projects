#ifndef parameters_h
#define parameters_h

//=====================================================================
// シリアルコンソールへのデバック出力
//      #define DEBUG = 出力あり
//　　//#define DEBUG = 出力なし（コメントアウトする）
//=====================================================================
#define DEBUG

//=====================================================================
// シリアル通信のボーレート
//=====================================================================
#define SERIAL_BAUD 115200

//=====================================================================
// スリープ時間、送信時間の設定
//  DEFAULT_SLEEP_INTERVAL      : スリープ時間 (秒)
//  DEFAULT_WAKE_INTERVAL       : Beacon送信時間 (秒)
//  DEFAULT_CLICK_WAKE_INTERVAL : ダブルタップをしたときの起動時間 (秒)
//=====================================================================
#define DEFAULT_SLEEP_INTERVAL      60
#define DEFAULT_WAKE_INTERVAL       1
#define DEFAULT_CLICK_WAKE_INTERVAL 20

//=====================================================================
// センサ測定間隔、データ保存間隔の設定
//  DEFAULT_SENS_FREQ : センサON頻度
//  DEFAULT_SAVE_FREQ : データ保存頻度
//=====================================================================
#define DEFAULT_SENS_FREQ 1
#define DEFAULT_SAVE_FREQ 1

//=====================================================================
// IOピンの定義
//=====================================================================
// for STM32 29-pin leaf
#define BLE_WAKEUP PB12 // D7   PB12
#define BLE_RX     PA0  // [A2] PA1
#define BLE_TX     PA1  // [A1] PA0
#define INT_0      PC7  // INT0
#define INT_1      PB3  // INT1

// for STM32 58-pin leaf Bus-A
// #define BLE_WAKEUP PA8
// #define BLE_TX     PA1
// #define BLE_RX     PA0
// #define INT_0      PC7
// #define INT_1      PB3

// for STM32 58-pin leaf Bus-B
// #define BLE_WAKEUP PB11
// #define BLE_TX     PC5
// #define BLE_RX     PC4
// #define INT_0      PC7
// #define INT_1      PB3

//=====================================================================
// プログラム内で使用する定数
//=====================================================================
// I2C addresses
#define LIS2DH_ADDRESS        0x19 // Accelerometer (SD0/SA0 pin = VCC)
#define OPT3001_ADDRESS       0x45 // Ambient Light Sensor (ADDR pin = VCC)
#define LCD_I2C_EXPANDER_ADDR 0x1A // LCD I2C Expander
#define BATT_ADC_ADDR         0x50 // Battery ADC

// Ring buffer
// These addresses must not overlap with the bootloader area
#define RING_BUFF_START_ADDR 0x00020000
#define RING_BUFF_END_ADDR   RING_BUFF_START_ADDR + 65535

// Adjust this number for the sensitivity of the 'click' force
// this strongly depend on the range! for 16G, try 5-10
// for 8G, try 10-20. for 4G try 20-40. for 2G try 40-80
#define CLICK_THRESHOLD 80
#define SINGLE_TAP 1
#define DOUBLE_TAP 2

// BLE states
#define BLE_STATE_STANDBY          (0)
#define BLE_STATE_SCANNING         (1)
#define BLE_STATE_ADVERTISING      (2)
#define BLE_STATE_CONNECTING       (3)
#define BLE_STATE_CONNECTED_MASTER (4)
#define BLE_STATE_CONNECTED_SLAVE  (5)

// BLE Connecting Mode
#define MODE_IDLE               (0)
#define MODE_SEND_DATA          (1)
#define MODE_CLEAR_EEPROM       (2)

// BLE Advertising Interval
// Example: If the minimum advertisement interval is 40ms and the maximum advertisement interval is 100ms
// then the real advertisement interval will be mostly the middle value (70ms) plus a randomly added 20ms delay,
// which needs to be added according to the Bluetooth specification.
#define MIN_ADV_INTVAL 200 // 200 * 0.625ms = 125ms
#define MAX_ADV_INTVAL 400 // 400 * 0.625ms = 250ms

#endif // parameters_h
