#include <Arduino.h>
#include <SPI.h>
#include <STM32LowPower.h>
#include <WiFi101Leafony.h>
#include <driver/source/nmasic.h>
#include <TCA9536.h>
#include "arduino_secrets.h"

// WiFi101LeafonyをSTM32で動作させる場合に必要な記述
extern "C" void attachInterruptMultiArch(uint32_t pin, void *chip_isr, uint32_t mode)
{
  void (*_c)(void) = (void (*)(void))(chip_isr);
  attachInterrupt(pin, _c, mode);
}
extern "C" void detachInterruptMultiArch(uint32_t pin)
{
  detachInterrupt(pin);
}

/********************
 * IOエキスパンダー
 ********************/
#define IOEX_CHIP_EN_PIN 0
#define IOEX_WAKE_PIN 1

TCA9536 io;

/********************
 * Wi-Fi
 * SSIDとパスワードを src/arduino_secrets.h に記述してください
 ********************/
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS; // the WiFi radio's status

// Specify IP address or hostname
String hostName = "www.google.com";
int pingResult;

/********************
 * プロトタイプ宣言
 ********************/
void printWiFiData();
void printCurrentNet();
void printMacAddress(byte mac[]);
void attachInterruptMultiArch(uint32_t pin, void *chip_isr, uint32_t mode);
void detachInterruptMultiArch(uint32_t pin);
void wifiDozeMode();
void wifiPowerDown();

/**
 * @brief メイン初期化処理
 *
 */
void setup()
{
  // Initialize serial
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  LowPower.begin(); // STM32 Low Power Libraryを初期化
  Wire.begin();     // I2Cを初期化

  // IOエキスパンダーを初期化
  if (io.begin() == false)
  {
    Serial.println("TCA9536 not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }
  io.pinMode(IOEX_WAKE_PIN, OUTPUT);
  io.pinMode(IOEX_CHIP_EN_PIN, OUTPUT);

  // WiFiピンを初期化
  WiFi.setPins(D10, D5, A4, -1);       //  WiFi用のピンアサインを設定: CS = D10, IRQ = D5, RESET_N = A4, CHIP_EN = N.A.
  SPI.setClockDivider(SPI_CLOCK_DIV8); // WiFi用SPIの通信速度を設定
  io.write(IOEX_WAKE_PIN, HIGH);       // WAKEを初期化
  io.write(IOEX_CHIP_EN_PIN, HIGH);    // CHIP_ENを初期化

  // Check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi101 shield: NOT PRESENT");
    while (true)
      ;
  }

  // Print firmware version on the shield
  String fv = WiFi.firmwareVersion();
  String latestFv;
  Serial.print("Firmware version installed: ");
  Serial.println(fv);

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 5 seconds for connection:
    delay(5000);
  }

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");
  printCurrentNet();
  printWiFiData();
}

/**
 * @brief メインループ
 *
 */
void loop()
{
  Serial.print("Pinging ");
  Serial.print(hostName);
  Serial.print(": ");

  pingResult = WiFi.ping(hostName);

  if (pingResult >= 0)
  {
    Serial.print("SUCCESS! RTT = ");
    Serial.print(pingResult);
    Serial.println(" ms");
  }
  else
  {
    Serial.print("FAILED! Error code: ");
    Serial.println(pingResult);
  }

  wifiDozeMode();

  Serial.flush();
  LowPower.deepSleep(10 * 1000);
}

/**
 * @brief
 *
 */
void printWiFiData()
{
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP address : ");
  Serial.println(ip);

  Serial.print("Subnet mask: ");
  Serial.println((IPAddress)WiFi.subnetMask());

  Serial.print("Gateway IP : ");
  Serial.println((IPAddress)WiFi.gatewayIP());

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

/**
 * @brief
 *
 */
void printCurrentNet()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI): ");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type: ");
  Serial.println(encryption, HEX);
  Serial.println();
}

/**
 * @brief
 *
 * @param mac
 */
void printMacAddress(byte mac[])
{
  for (int i = 5; i >= 0; i--)
  {
    if (mac[i] < 16)
    {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0)
    {
      Serial.print(":");
    }
  }
  Serial.println();
}

/**
 * @brief Dozeモードに移行
 * @details WiFiモジュールのコンフィギュレーションを維持したまま消費電流400uA未満の低電力モードに移行する
 */
void wifiDozeMode()
{
  WiFi.maxLowPowerMode();
}

/**
 * @brief WiFiモジュールの電源を切る
 * @details WiFiモジュールの電源を切り、消費電流を4uA未満の最小限に抑える
 */
void wifiPowerDown()
{
  io.write(IOEX_CHIP_EN_PIN, LOW);
}
