//=====================================================================
//    (c) 2023 LEAFONY SYSTEMS Co., Ltd
//    Released under the MIT license
//    https://opensource.org/licenses/MIT
//
//      Rev.00 2023/02/01  First release
//=====================================================================
#include <Arduino.h>

void SystemClock_Config(void);  // CPU周波数を設定
void SystemClock_Print(void);   // CPU周波数を表示

void setup() {
  Serial.begin(115200);
  delay(10);
  SystemClock_Config();         // CPU周波数を設定
}

void loop(){
  SystemClock_Print();          // CPU周波数を表示
  delay(1000);
}
