# ststm32@16.1.0を使用する際のボードファイルの設定方法
ststm32@16.1.0には、LEAFONY_AP03のボードファイルが登録されていないので、手動で登録する必要があります。
[STM32MCUのボード・ファイル](https://docs.leafony.com/docs/environment/stm32/platformio/#stm32mcu%E3%81%AE%E3%83%9C%E3%83%BC%E3%83%89%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB%E5%8F%82%E8%80%83)を読んで頂き、以下の手順で設定して下さい。</br>
1.Nucleo L452REのプロジェクト(Test)を立ち上げます。</br>
2.platform をststm32@16.1.0に変更し、ビルドすると、以下のファイルがインストールされます。</br>
  *platforms/ststm32@16.1.0</br>
  *packages/toolchain-gccarmnoneeabi@1.100301.220327</br>
  *packages/framework-arduinoststm32@4.20500.230714</br>
3.Leafony_AP03とLeafony_AP03.jsonをダウンロードし、所定の場所に保存します。</br>
