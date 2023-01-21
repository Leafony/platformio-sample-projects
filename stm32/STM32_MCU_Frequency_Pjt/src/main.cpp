//=====================================================================
//    (c) 2023 LEAFONY SYSTEMS Co., Ltd
//    Released under the MIT license
//    https://opensource.org/licenses/MIT
//
//      Rev.00 2023/02/01  First release
//=====================================================================
#include <Arduino.h>

// prototype
void SystemClock_Config(void);  // CPU frequency setting
void SystemClock_Print(void);   // CPU frequency display

void setup() {
  Serial.begin(115200);
  delay(10);
  SystemClock_Config();         // CPU frequency setting
}

void loop(){
  SystemClock_Print();          // CPU frequency display
  delay(1000);
}
