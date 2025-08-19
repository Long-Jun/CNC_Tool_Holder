/*
  Copyright (c) 2025 National Taipei University of Technology. All rights reserved.
  Copyright (c) 2025 Long Jun, Chiou. All rights reserved.

  Author     : Long Jun, Chiou
  Co-Author  : Chun Ru, Tang
  Website    : https://github.com/Long-Jun
  Date       : 2025-08-20

  Description:
  This Arduino sketch is developed by NTUT UTL & SPIE Lab. It is designed for the
  Toolholder Roughness Gauge (CNC_v3.2 PCB) using Seeed XIAO nRF52840 Sense.
  
  Target board: Seeed XIAO nRF52840 Sense (please select the correct board in Arduino IDE)

  ✅ Features | 功能說明：
  - High-speed read of LDC1614 channel 0 and 1 over I2C
  - Output measurement data to USB serial port (115200 baud)
  - Adjustable output interval via UTL_Timer's Print_USB.every(ms)

  ✅ 附註 Notes:
  - 實際資料更新頻率受限於 LDC1614 的轉換速度
  - 雙通道最大更新頻率約為 18Hz（非 USB 傳輸瓶頸）

  
  本程式由臺北科技大學 UTL & SPIE Lab 開發，適用於刀把粗度計 CNC_v3.2 PCB，

  開發板：Seeed XIAO nRF52840 Sense（請於 Arduino IDE 中選擇正確板型）
  適用裝置：刀把粗度計 CNC_v3.2 PCB
*/

#include <Wire.h>
#include "src/UTL_LDC1614.h"
#include "src/UTL_Timer.h"

#define RP 0.631
#define INDUCTANCE 1.5458
#define CAPATANCE 1000.0
#define Q 16.04

#define TAMP_ALERT 6
#define LDC_INT 7
#define LDC_SD 8

#define TMP275_ADDR 0x48      // 根據 A0~A2 腳位，0x48 為預設位址
#define CONFIG_REG  0x01      // Configuration register
#define TEMP_REG    0x00      // Temperature register

UTL_LDC1614_Inductive ldc;

uint16_t value = 0;
uint32_t ChannelData[4];

UTL_Timer Print_USB;


void setup() {

  //設定各 pin 功能
  pinMode(TAMP_ALERT, INPUT);
  pinMode(LDC_INT, INPUT);
  pinMode(LDC_SD, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  delayMicroseconds(10000);

  digitalWrite(LDC_SD, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  delayMicroseconds(10000);

  Serial.begin(115200);
  //while (!Serial) delay(10);

  Wire.begin();
  Wire.setClock(400000);
  delayMicroseconds(10000);
  ldc.set_iic_addr(0x2B);

  /*Multiple channel use case configuration.*/
  if (ldc.LDC1614_mutiple_channel_config(RP, INDUCTANCE, CAPATANCE, Q))
  {
    Serial.println("can't detect sensor!");
    while (1) delay(100);
  }

  Print_USB.every(10, print_value);
}

void loop() {

  ldc.IIC_read_16bit(LDC1614_READ_DEVICE_ID, &value);

  if (value == 0x3055) {
    ldc.LDC1614_get_channel_result(0, &ChannelData[0]);
    ldc.LDC1614_get_channel_result(1, &ChannelData[1]);
  }
  else {
    Serial.println("The IIC communication fails,please reset the sensor!");
    delay(500);
  }

  Print_USB.update();
}

void print_value() {
  Serial.print(ChannelData[0]);
  Serial.print("\t");
  Serial.print(ChannelData[1]);
  Serial.println();
}
