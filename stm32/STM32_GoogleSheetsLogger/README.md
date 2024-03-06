# Google スプレッドシートでセンサデータの可視化
## はじめに
Basic Kit2とWi-Fi Maryを使って、環境センサが出来上がります。この環境センサをインターネットに繋げば、Googleスプレッドシートでのセンサデータの可視化が簡単に出来ます。

## 用意するもの
* Basic Kit2
* AC06 Wi-Fi Mary
* AV04 2V～4.5V
* AZ67 AAA battery holder
* AZ63 Nut Plate
* 単4ニッケル⽔素電池x3本  
* Wi-Fiルータ

## Google Apps Scriptの設定
下記に従って Google App Script を設定します。
https://docs.leafony.com/docs/examples/advanced/2_p/esp32/esp32_googlesheets_1/#google-apps-script%E3%81%AE%E8%A8%AD%E5%AE%9A

## Google Apps Scriptのデプロイ
下記に従って、Google Apps Scriptのデプロイをします。
https://docs.leafony.com/docs/examples/advanced/2_p/esp32/esp32_googlesheets_1/#google-apps-script%E3%81%AE%E3%83%87%E3%83%97%E3%83%AD%E3%82%A4

## Leafonyの組み立て
4-Sensors、USB、STM32MCU、Wi-Fi Mary、2V～4.5Vリーフの順番で組み立てます。

## ソースコード
`arduino_secrets.h` に、Wi-FiルータのSSIDとパスワード、Google Apps ScriptのデプロイIDを入力して、書き込みます。

## 実行結果
Google スプレッドシートにセンサデータが書き込まれたことが確認できます。
