#include <Arduino.h>

void SystemClock_Config(void);  // CPU周波数を設定
void SystemClock_Print(void);   // CPU周波数を表示

void setup() {
  Serial.begin(115200);
  delay(10);
  SystemClock_Config();         // CPU周波数を設定
}

void loop(){
  Serial.println("Hello World");
  SystemClock_Print();          // CPU周波数を表示
  delay(1000);
}