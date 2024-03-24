# ststm32のボードのバージョンに関して

https://github.com/platformio/platform-ststm32/tree/v16.1.0




## はじめに
Basic Kit2とWi-Fi Maryを使って、環境センサが出来上がります。この環境センサをインターネットに繋げば、Googleスプレッドシートでのセンサデータの可視化が簡単に出来ます。

## 用意するもの
* Basic Kit2
* AC06 Wi-Fi Mary
* AV04 2V～4.5V
* AX04 Spacer
* AZ67 AAA battery holder
* AZ63 Nut Plate
* 単4ニッケル⽔素電池x3本  
* Wi-Fiルータ

[env:LEAFONY_AP03]
platform = ststm32@16.1.0
board = LEAFONY_AP03
framework = arduino
monitor_speed = 115200
