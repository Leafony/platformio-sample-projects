//=====================================================================
//    (c) 2023 LEAFONY SYSTEMS Co., Ltd
//    Released under the MIT license
//    https://opensource.org/licenses/MIT
//
//      Rev.00 2023/02/01  First release
//=====================================================================
#include <Arduino.h>

int pushButton = 0;

void setup() {
  Serial.begin(115200);
  pinMode(pushButton, INPUT);
}

void loop() {
  int buttonState = digitalRead(pushButton);
  Serial.println(buttonState);
  delay(1000);
}