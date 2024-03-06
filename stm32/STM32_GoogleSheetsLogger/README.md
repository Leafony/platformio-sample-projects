# Google スプレッドシートでセンサデータの可視化
## 用意するもの
* ESP32 Wi-Fi Kit2
* 4-Sensors(温度・湿度・照度)
* 単4ニッケル⽔素電池x3本
* 開発用パソコン
* Wi-Fiルータ




## 動作手順

1. 下記の記述に従い Google App Script を設定した Google Sheets を用意する
https://docs.leafony.com/docs/examples/advanced/2_p/esp32/esp32_googlesheets_1/#google-apps-script%E3%81%AE%E8%A8%AD%E5%AE%9A

2. `arduino_secrets.h` に、1で作成したシートのシートIDと、Wi-Fiの接続情報を入力

3. 4-Sensorsリーフ、STM32リーフ、USBリーフ、WiFiリーフを接続

4. スケッチを書き込み
