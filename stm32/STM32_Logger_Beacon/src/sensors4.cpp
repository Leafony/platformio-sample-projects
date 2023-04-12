#include "sensors4.h"
#include "parameters.h"

ClosedCube_OPT3001 light;
Adafruit_LIS3DH accel = Adafruit_LIS3DH();

//--------------------
// 温度センサ2点補正用データ
//--------------------
// 温度補正用データ0
const float TL0 = 25.0;     // 4-Sensors温度測定値
const float TM0 = 25.0;     // 温度計等測定値
// 温度補正用データ1
const float TL1 = 40.0;     // 4-Sensors温度測定値
const float TM1 = 40.0;     // 温度計等測定値
// 湿度補正用データ0
const float HL0 = 60.0;     // 4-Sensors湿度測定値
const float HM0 = 60.0;     // 湿度計等測定値
// 湿度補正用データ1
const float HL1 = 80.0;     // 4-Sensors湿度測定値
const float HM1 = 80.0;     // 湿度計等測定値

/**
 * @brief センサリーフを初期化する
 */
void setupSensors() {
  // LIS2DH (accelerometer)
  accel.begin(LIS2DH_ADDRESS);
  accel.setRange(LIS3DH_RANGE_2_G);             // Full scale +/- 2G
  accel.setDataRate(LIS3DH_DATARATE_50_HZ);     // Data rate = 10Hz
  accel.setClick(DOUBLE_TAP, CLICK_THRESHOLD);  // enable click interrupt

  // HTS221 (temperature /humidity)
  smeHumidity.begin();

  // OPT3001 (light)
  OPT3001_Config newConfig;
  OPT3001_ErrorCode errorConfig;
  light.begin(OPT3001_ADDRESS);
  newConfig.RangeNumber = B1100;             // automatic full scale
  newConfig.ConvertionTime = B0;             // convertion time = 100ms
  newConfig.ModeOfConversionOperation = B01; // single-shot conversion
  newConfig.Latch = B0;                      // hysteresis-style
  errorConfig = light.writeConfig(newConfig);

  delay(100); // wait until all sensors are ready
}

/**
 * @brief センサを読み出す
 */
void readSensors(float* temp, float* humid, float* illum, float* batt) {
  float dataTemp, dataHumid;
  uint8_t adcVal1 = 0;
  uint8_t adcVal2 = 0;

  smeHumidity.begin();
  dataTemp = (float)smeHumidity.readTemperature();
  dataHumid = (float)smeHumidity.readHumidity();

  dataTemp=TM0+(TM1-TM0)*(dataTemp-TL0)/(TL1-TL0);      // 温度補正
  dataHumid=HM0+(HM1-HM0)*(dataHumid-HL0)/(HL1-HL0);    // 湿度補正

  if (dataHumid >= 100) dataHumid=100;

  *temp = dataTemp;
  *humid = dataHumid;

  delay(100);

  OPT3001 result = light.readResult();
  *illum = result.lux;

  // ADC081C027（ADC) battery voltage
  Wire.beginTransmission(BATT_ADC_ADDR);
  Wire.write(0x00);
  Wire.endTransmission(false);
  Wire.requestFrom(BATT_ADC_ADDR, 2);
  adcVal1 = Wire.read();
  adcVal2 = Wire.read();

  // 測定値がFFならバッテリリーフはつながっていない
  if (adcVal1 == 0xff && adcVal2 == 0xff) {
    adcVal1 = adcVal2 = 0;
  }

  // 電圧計算 ADC * ((リファレンス電圧(3.3V)/ ADCの分解能(256)) * 分圧比 (2倍))
  double temp_mv = ((double)((adcVal1 << 4) | (adcVal2 >> 4)) * 3300 * 2) / 256;
  *batt = (float)(temp_mv / 1000);
}

/**
 * @brief センサを起動する
 */
void wakeupSensors() {
  smeHumidity.activate();

  OPT3001_Config newConfig;
  OPT3001_ErrorCode errorConfig;
  newConfig.RangeNumber = B1100;  // automatic full scale
  newConfig.ModeOfConversionOperation = B01; //single-shot conversion
  errorConfig = light.writeConfig(newConfig);
  delay(300);
}

/**
 * @brief センサをスリープさせる
 */
void sleepSensors() {
  smeHumidity.deactivate();

  OPT3001_Config newConfig;
  OPT3001_ErrorCode errorConfig;
  newConfig.ModeOfConversionOperation = B00;
  errorConfig = light.writeConfig(newConfig);
}

/**
 * @brief 加速度センサ割り込みを有効化する
 */
void enableAccelInterrupt() {
  accel.getClick();
}
