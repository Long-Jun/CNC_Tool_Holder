/*
  Copyright (c) 2025 National Taipei University of Technology. All rights reserved.
  Copyright (c) 2025 Long Jun, Chiou. All rights reserved.

  Author     : Long Jun, Chiou
  Co-Author  : Chun Ru, Tang
  Website    : https://github.com/Long-Jun
  Date       : 2025-08-20

  Description:
  This Arduino sketch is developed by NTUT UTL & SPIE Lab. It is designed for the
  Toolholder Roughness Gauge (CNC_v3.2 PCB) using Seeed XIAO nRF52840 Sense with BLE.
  
  This version reads LDC1614 sensor data and broadcasts processed results via
  Bluetooth Low Energy (BLE) advertising packets. The system includes baseline
  calibration and uses timer-based scheduling for efficient execution.

  ✅ Features | 功能說明：
  - 定時讀取 LDC1614 第 0 與第 1 通道感測數值
  - 進行零點校正（Baseline Calibration）以去除偏移
  - 感測資料透過 BLE 廣播封包發送，不需配對、不需連線
  - 所有任務（感測、傳輸）皆以 UTL_Timer 定時排程管理

  ✅ 附註 Notes:
  - 廣播資料以 Manufacturer Data 欄位形式嵌入數值
  - 可依實驗需求調整感測與廣播頻率（例如：10ms、40ms 等）
  - 本版本適合用於定點量測、不依賴藍牙連線的應用場景

  開發板：Seeed XIAO nRF52840 Sense（請於 Arduino IDE 中選擇正確板型）
  適用裝置：刀把粗度計 CNC_v3.2 PCB，支援雙通道感測線圈
*/

#include <Wire.h>
#include <bluefruit.h>
#include "src/UTL_LDC1614.h"
#include "src/UTL_Timer.h"

#define RP 2.1
#define INDUCTANCE 1.5
#define CAPATANCE 1000.0
#define Q 17.4

UTL_Timer LDC_Read_Timer;
UTL_Timer Print_Timer;
UTL_Timer BLE_Timer;

UTL_LDC1614_Inductive ldc;

bool LDC_status = false;

uint32_t ChannelBuf[4];
// 用來儲存零點校正的偏移值
uint32_t baseline[4] = {0};

void setup()
{
  Serial.begin(115200);
  //while (!Serial) delay(10);

  Wire.begin();
  Wire.setClock(400000);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("UTL LDC TEST");

  // Beacon 初始化 副程式
  BLE_init();

  // 設定LDC1614 Address
  ldc.set_iic_addr(0x2B);

  // 重置LDC1614確保初始化資料正常
  ldc.LDC1614_reset_sensor();

  // 配置LDC1614線圈參數(四通道)
  if (ldc.LDC1614_mutiple_channel_config(RP, INDUCTANCE, CAPATANCE, Q) != 0)
  {
    Serial.println("can't detect LDC Coil");
    while (1) delay(100);
  }
  // 呼叫校正函數，取得各通道的零點校正值
  calibrateSensors();

  // 固定時間執行 LDC 讀取副程式
  LDC_Read_Timer.every(10, ldc_read);

  // 固定時間執行BLE傳送副程式
  BLE_Timer.every(40, BLE_Transmitter);

  // 固定時間執行數值印出副程式
  Print_Timer.every(20, print_sensor_value);
}

void loop()
{
  LDC_Read_Timer.update();
  BLE_Timer.update();
  //Print_Timer.update();
}

// 校正函數：讀取多次取平均作為零點偏移值
void calibrateSensors() {
  const int samples = 10;
  uint32_t sum[2] = {0};
  uint32_t temp[2] = {0};

  Serial.println("開始校正，請保持環境穩定...");
  // 延時確保系統穩定
  delay(1000);

  for (int j = 0; j < samples; j++) {
    // 讀取 LDC 數值 (通道0~3)
    for (uint8_t i = 0; i <= 1; i++) {
      delayMicroseconds(1000);
      ldc.LDC1614_get_channel_result(i, &temp[i]);
    }
    // 累加各通道數值
    for (int i = 0; i < 2; i++) {
      sum[i] += temp[i];
    }
  }
  // 取平均作為基準值
  for (int i = 0; i < 2; i++) {
    baseline[i] = sum[i] / samples;
  }
  Serial.println("校正完成！");
}

void ldc_read() {
  uint16_t value = 0;

  // 確認 LDC 健康狀態以及是否在線上 true = online, false = offline
  ldc.IIC_read_16bit(LDC1614_READ_DEVICE_ID, &value);
  if (value != 0x3055) {
    ldc.LDC1614_reset_sensor();
    delay(10);
  }

  // 讀取 LDC 數值 (存放於 ChannelBuf[0~3])
  for (uint8_t i = 0; i <= 1; i++) {
    delayMicroseconds(1000);
    ldc.LDC1614_get_channel_result(i, &ChannelBuf[i]);
  }
}

void print_sensor_value() {
  for (int i = 0; i < 2; i++) {
    // 印出減除零點校正後的結果
    Serial.printf("ch%d:", i);
    //Serial.print((int32_t)ChannelBuf[i]);
    Serial.print((int32_t)ChannelBuf[i] - (int32_t)baseline[i]);
    Serial.print("\t");
  }
  Serial.println();
}

void BLE_init() {
  Bluefruit.begin();
  Bluefruit.setTxPower(4);
  Bluefruit.setName("UTL");

  // 清除先前的廣播資料
  Bluefruit.Advertising.clearData();

  // 加入基本廣播欄位
  // 設定 Flags：LE only, General Discoverable Mode
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  // 加入 TX Power 資訊
  Bluefruit.Advertising.addTxPower();

  // 加入裝置名稱 (0x09)
  Bluefruit.Advertising.addName();

  // 讀取 A0 並拆成 2 個位元組作為 Manufacturer Data
  //int sensorValue = 0x69;
  int sensorValue = analogRead(A0);
  uint8_t mfgData[2];
  mfgData[0] = sensorValue & 0xFF;
  mfgData[1] = (sensorValue >> 8) & 0xFF;
  Bluefruit.Advertising.addManufacturerData(mfgData, 2);

  // 設定廣播間隔 (單位：0.625ms)，此例約 20ms~152.5ms
  Bluefruit.Advertising.setInterval(32, 32);

  // 啟動廣播
  Bluefruit.Advertising.start();
  Serial.println("Started Advertising");
}

void BLE_Transmitter() {

  int sensorValue = ((int32_t)ChannelBuf[0] - (int32_t)baseline[0]) / 1000.0;

  // 更新廣播：先停止、清除，再重新加入各欄位資料
  Bluefruit.Advertising.stop();
  Bluefruit.Advertising.clearData();
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addName();

  uint8_t mfgData[2];
  mfgData[0] = sensorValue & 0xFF;
  mfgData[1] = (sensorValue >> 8) & 0xFF;
  Bluefruit.Advertising.addManufacturerData(mfgData, 2);

  Bluefruit.Advertising.start();

  Serial.print("Coil Value: ");
  Serial.println(sensorValue);

}
