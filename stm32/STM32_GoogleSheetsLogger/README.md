# Google スプレッドシートでセンサデータの可視化
## はじめに
Basic Kit2とWi-Fi Maryを使って、環境センサが出来上がります。この環境センサをインターネットに繋げば、Googleスプレッドシートでのセンサデータの可視化が簡単に出来ます。

## 用意するもの
* Basic Kit2
* AC06 Wi-Fi Mary
* AV04 2V～4.5V
* AZ67 AAA battery holder
* 単4ニッケル⽔素電池x3本  
* Wi-Fiルータ

## 

1. 下記の記述に従い Google App Script を設定した Google Sheets を用意する
https://docs.leafony.com/docs/examples/advanced/2_p/esp32/esp32_googlesheets_1/#google-apps-script%E3%81%AE%E8%A8%AD%E5%AE%9A

2. `arduino_secrets.h` に、1で作成したシートのシートIDと、Wi-Fiの接続情報を入力

3. 4-Sensorsリーフ、STM32リーフ、USBリーフ、WiFiリーフを接続

4. スケッチを書き込み
