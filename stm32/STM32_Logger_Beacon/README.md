# STM32 Logger Beacon

STM32リーフで温度、湿度、照度のログを一定間隔で測定し、オンチップのFlash ROMにログデータを保存するサンプルです。

## ログ取得手順

1. スケッチを書き込みLeafonyの電源スイッチをONにする
2. PCやスマートフォン等で[STM32 Sensors Logger Webアプリケーション](https://docs.leafony.com/Sample-Sketches/STM32_Logger_Beacon/sample_web_app/lescan/)を開き、LeafonyとBluetoothで接続する（このときLeafonyの時刻がPCと同期します。）
4. Bluetoothを切断し、しばらく放置する。このとき一定間隔でログが保存されています。
5. LeafonyとPCをUSBで接続し、TeraTerm等のシリアルコンソールで　１１５２００ｂｐｓでUART接続する。
6. `s`と送信するとログデータがダンプされる。

## ログ保存領域の変更方法

`src/parameter.h`の下記の部分を変更し、任意のサイズに変更可能です。
```c
#define RING_BUFF_END_ADDR   RING_BUFF_START_ADDR + 65535 // 64kB
```
