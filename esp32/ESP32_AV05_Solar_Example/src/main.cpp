//=====================================================================
//  Leafony Platform sample sketch
//     Platform     : ESP32
//     Processor    : ESP32-WROOM-32
//     Application  : Sending Sensor Data to Google Sheets
//
//     Leaf configuration
//       (1) AP02 ESP MCU
//       (2) AI01 4-Sensors
//
//    (c) 2020 Trillion-Node Study Group
//    Released under the MIT license
//    https://opensource.org/licenses/MIT
//
//      Rev.00 2020/8/17  First release
//=====================================================================
#include <Arduino.h>

// #define ENTERPRISE

#ifdef ENTERPRISE // Enterprise
#include <esp_wpa2.h>
#endif

#include "main.h"
#include "ai01_4sensors.h"
#include "av05_solar_charger.h"

// Unique ID
String UniqueID = "Leafony_A";

#ifdef ENTERPRISE                        // Enterprise
#define EAP_IDENTITY "YOUR_EAP_IDENTITY" // identity
#define EAP_PASSWORD "YOUR_EAP_PASSWORD" // Password
const char *SSID_ENT = "YOUR_EAP_SSID";  // Wi-Fi SSID
#endif

// Connecting WiFi Settings
const char *SSID = "YOUR_WIFI_SSID";         // WiFi SSID
const char *PASSWORD = "YOUR_WIFI_PASSWORD"; // WiFi Password

// Accessed Google Script Settings
const char *APP_SERVER = "script.google.com";
const char *KEY = "YOUR_SHEET_KEY";

// Device sleep time (sec) to reduce Joule heat
uint64_t SLEEP_TIME_SEC = 60;

/*********************************************************************
 * グローバル変数
 *********************************************************************/
hw_timer_t *timer0 = NULL; // 電源投入後のLED消灯用タイマー
hw_timer_t *timer1 = NULL; // 電源ボタン押下時のLED点滅用タイマー

volatile bool b_psw_pushed = false; // 電源ボタンが押されたフラグ

volatile bool b_timer0_timeout = false;     // タイマー0タイムアウトフラグ
volatile uint32_t count_timer0_timeout = 0; // タイマー0タイムアウトカウンタ

volatile bool b_timer1_timeout = false; // タイマー1タイムアウトフラグ

volatile bool b_power_on = false; // 電源ONフラグ(電源投入処理が終わるとtrueになる)

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

/**
 * @brief バッテリーのステータスを表示
 *
 */
float checkBatteryStatus()
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
  Serial.print("Battery  = " + String(dataBatt));
  Serial.print(", ");

  int pws = digitalRead(INT_PSW_N_PIN);
  Serial.println("PWS = " + String(pws));

  return dataBatt;
}

/**
 * @brief
 *
 * @param temperature
 * @param humidity
 * @param illumination
 * @param dataBatt
 */
void accessToGoogleSheets(float temperature, float humidity, float illumination, float dataBatt)
{
  HTTPClient http;
  String URL = "https://script.google.com/macros/s/";

  URL += KEY;
  URL += "/exec?";
  URL += "UniqueID=";
  URL += UniqueID;
  URL += "&temperature=";
  URL += temperature;
  URL += "&humidity=";
  URL += humidity;
  URL += "&illumination=";
  URL += illumination;
#if 1
  URL += "&Battery=";
  URL += dataBatt;
#endif

  Serial.println("[HTTP] begin...");
  Serial.println(URL);
  // access to your Google Sheets
  Serial.println();
  // configure target server and url
  http.begin(URL);

  Serial.println("[HTTP] GET...");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    Serial.print("[HTTP] GET... code: ");
    Serial.println(httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      Serial.println(payload);
    }
  }
  else
  {
    Serial.print("[HTTP] GET... failed, error: ");
    Serial.println(http.errorToString(httpCode).c_str());
  }
}

/**
 * @brief ESP32 start light sleep
 *
 */
void espLightSleep(int period)
{
  Serial.print("sleeping ");
  Serial.print(period);
  Serial.println(" sec...");
  Serial.flush();

  // 電源ボタン割り込みを無効化
  detachInterrupt(INT_PSW_N_PIN);

  // タイマー割り込みでウェイクアップ
  esp_sleep_enable_timer_wakeup(period * 1000 * 1000); // set deep sleep time
  // 電源ボタンが押された時のウェイクアップを有効化
  // 電源ボタンが押されたら電源LEDの処理をするために必要
  gpio_wakeup_enable((gpio_num_t)INT_PSW_N_PIN, GPIO_INTR_LOW_LEVEL);
  esp_sleep_enable_gpio_wakeup();
  // スリープ
  esp_light_sleep_start();
}

/**
 * @brief ESP32 start deep sleep
 * After calling "esp_deep_sleep_start" function, any following codes will not be executed
 * When restarting ESP32, all variables are restored and the program will start from the beginning
 */
void espDeepSleep(int period)
{
  Serial.print("sleeping ");
  Serial.print(period);
  Serial.println(" sec...");
  Serial.flush();
  esp_sleep_enable_timer_wakeup(period * 1000 * 1000); // set deep sleep time
  esp_deep_sleep_start();                              // enter deep sleep
}

void mainTask()
{
  // センサデータを読み出し
  float temperature = readTemperature();
  float humidity = readHumidity();
  float illumination = readIllumination();
  float dataBatt = readBatteryVoltage();

  Serial.print("\ntemperature : ");
  Serial.println(temperature);
  Serial.print("humidity    : ");
  Serial.println(humidity);
  Serial.print("illumination  : ");
  Serial.println(illumination);
  Serial.print("Battery  : ");
  Serial.println(dataBatt);

  // send sensor values to google sheets
#if 1
  accessToGoogleSheets(temperature, humidity, illumination, dataBatt);
#else
  accessToGoogleSheets(temperature, humidity, illumination);
#endif

  // WiFi Connection killed
  Serial.println("\nWiFi is disconnected");
  WiFi.disconnect();
}

/************************************************************************
 * 初期化
 ************************************************************************/
void setup()
{
  // WiFi connection controlling parameters
  int statusCheckCounter = 0;
  const int CHECK_NUM_MAX = 100;

  Serial.begin(115200);

  // センサリーフを初期化
  initSensors();

  // ソーラーチャージャーリーフを初期化
  if (!initAv05SolarCharger())
  {
    return;
  }

  // 割り込みピンを初期化
  pinMode(INT_PSW_N_PIN, INPUT);

  // 電源ボタンLEDを点灯
  turnOnLed();

  // 電源ボタンを一定期間後に消灯させるタイマーを設定
  timer0 = timerBegin(0, 80, true);                  // 80サイクルで1カウント = 1us
  timerAttachInterrupt(timer0, &onTimer0, true);     // 割り込み関数を登録
  timerAlarmWrite(timer0, 0.5 * 1000 * 1000, false); // 0.5秒間隔で割り込み
  timerAlarmEnable(timer0);                          // カウント開始

  // 電源ボタンを一定間隔で点滅させるタイマーを設定
  timer1 = timerBegin(1, 80, true);                  // 80サイクルで1カウント = 1us
  timerAttachInterrupt(timer1, &onTimer1, true);     // 割り込み関数を登録
  timerAlarmWrite(timer1, 0.5 * 1000 * 1000, false); // 0.5秒間隔で割り込み開始

  enable5VSupply(); // 5V出力をON

  /**
   * WiFiを初期化
   */
#ifdef ENTERPRISE // Enterprise
  WiFi.mode(WIFI_STA);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); // provide identity
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); // provide username --> identity and username is same
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)); // provide password
  esp_wifi_sta_wpa2_ent_enable();
  WiFi.begin(SSID_ENT); // connect to wifi
#else
  WiFi.begin(SSID, PASSWORD);
#endif

  Serial.print("WiFi connecting");
  // Wait until succeed connecting.
  // If the number of checks is more than CHECK_NUM_MAX, give up connecting and
  // start deepsleep to prevent Joule heat from affecting next measurements.
  while (WiFi.status() != WL_CONNECTED)
  {
    if (statusCheckCounter > CHECK_NUM_MAX)
    {
      WiFi.disconnect();
      Serial.println("failed");
      espDeepSleep(60);
    }
    delay(500);
    statusCheckCounter++;
  }
  Serial.println("\nconnected");
}

/************************************************************************
 * メインループ
 ************************************************************************/
void loop()
{
  // 電源ボタン割り込みを有効化
  attachInterrupt(INT_PSW_N_PIN, onPowerButtonPushed, RISING);

  /************************************************************************
   * 電源ON時のタイマー割り込み処理
   ************************************************************************/
  if (b_timer0_timeout == true) // 0.5秒でタイムアウト
  {
    b_timer0_timeout = false;
    count_timer0_timeout++;

    // 電源投入後、一定期間経過後、電源ボタンLEDを消灯
    if (count_timer0_timeout >= 3 * 2 /* 3秒 * 2Hz */)
    {
      // 電源ON処理完了
      b_power_on = true;

      // 電源ボタンLEDを消灯
      turnOffLed();
      count_timer0_timeout = 0;

      // タイマーを停止
      timerAlarmDisable(timer0);
    }

    timerWrite(timer0, 0); // タイマーをリセット
    timerAlarmEnable(timer0);
  }

  /************************************************************************
   * 電源ボタンが押された時の処理
   ************************************************************************/
  if (b_psw_pushed == true)
  {
    b_psw_pushed = false;
    Serial.println("Power button pushed!");
    delay(100); // チャタリング対策で一定時間待つ

    // LED点滅用タイマーを有効化
    timerAlarmEnable(timer1);
  }

  /************************************************************************
   * 電源OFF時のタイマー割り込み処理
   ************************************************************************/
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

    timerWrite(timer1, 0); // タイマーをリセット
    timerAlarmEnable(timer1);
  }

  /************************************************************************
   * 電源ボタンが押されていない場合のみメイン処理を実行しスリープ
   ************************************************************************/
  if (digitalRead(INT_PSW_N_PIN) == PSW_RELEASED && b_power_on == true)
  {
    // 電源ボタンLEDを消灯
    turnOffLed();

    // メイン処理を実行
    mainTask();

    // ESP32をスリープさせる
    // ウェイクアップ後再びloop関数から実行させるためlight sleepにする
    espLightSleep(SLEEP_TIME_SEC);
  }
}
