#include <Arduino.h>

//=====================================================================
//  Hello_World
//
//    (c) 2023 LEAFONY SYSTEMS Co., Ltd
//    Released under the MIT license
//    https://opensource.org/licenses/MIT
//
//      Rev.00 2023/02/01  First release
//=====================================================================
void setup() {
  Serial.begin(115200);
  delay(10);
}

void loop(){
  Serial.println("Hello World");
  delay(1000);
}