# Leafony LoRa Mary 受信機サンプル

Leafony LoRa Mary で信号を受信し、PCのシリアルモニタでデータを確認するサンプルです。
LoRa送信機サンプルと一緒にご利用ください。

## 使用するリーフ

以下のリーフを使用します。

| Type | Name | Q'ty |
|-|-|-|
|AC05|LoRa ABZ|1|
|AP01|AVR MCU|1|
|AZ01|USB|1|

## プロジェクトのビルド

このサンプルはPlatformIOプロジェクトになっています。[PlatformIO IDE for VSCodeの設定](https://docs.leafony.com/docs/environment/stm32/platformio/)の手順に従って環境を構築し、コードのビルドと書き込みを行ってください。

## 実行結果

LeafonyをPCに接続し、シリアルモニタを開くと受信したデータと、受信時の電波強度（RSSI 単位dBm）が表示されます。
