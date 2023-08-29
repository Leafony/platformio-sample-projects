# STM32 Logger Beacon

STM32リーフで温度、湿度、照度のログを一定間隔で測定し、オンチップのFlash ROMにログデータを保存するサンプルです。

## ログ保存領域の変更方法

`src/parameter.h`の下記の部分を変更し、任意のサイズに変更可能です。
```c
#define RING_BUFF_END_ADDR   RING_BUFF_START_ADDR + 65535 // 64kB
```
