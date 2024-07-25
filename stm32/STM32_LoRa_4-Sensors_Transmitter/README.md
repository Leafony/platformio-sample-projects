# LoRa + センサ 送信機サンプル

温度、湿度、照度、加速度といったセンサデータの値を読み出し、Leafony LoRa Maryでデータ送信を行うサンプルです。
LoRa受信機サンプルと一緒にご利用ください。

## 使用するリーフ

以下のリーフを使用します。

| Type | Name | Q'ty |
|-|-|-|
|AP04|4-Sensors|1|
|AC05|LoRa ABZ|1|
|AP01|AVR MCU|1|
|AZ01|USB|1|
|AV01|CR2032|1|

## プロジェクトのビルド

このサンプルはPlatformIOプロジェクトになっています。[PlatformIO IDE for VSCodeの設定](https://docs.leafony.com/docs/environment/stm32/platformio/)の手順に従って環境を構築し、、コードのビルドと書き込みを行ってください。

## 実行結果

電池を挿入し、電源スイッチをONにするとセンサーデータの送信が開始されます。
LoRa受信機をLeafonyをPCに接続し、シリアルモニタを開くと受信したデータが閲覧できます。
