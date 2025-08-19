/*
  Copyright (c) 2025 National Taipei University of Technology. All rights reserved.
  Copyright (c) 2025 Long Jun, Chiou. All rights reserved.

  Author     : Long Jun, Chiou
  Co-Author  : Chun Ru, Tang
  Website    : https://github.com/Long-Jun
  Date       : 2025-08-19

  Description:
  This sketch is part of the NTUT UTL & SPIE Lab's Arduino project archive.
  It was developed for the Toolholder Roughness Gauge (CNC_v3.2 PCB) using the
  Seeed XIAO nRF52840 Sense board.
  
  本程式由臺北科技大學 UTL & SPIE Lab 開發，適用於刀把粗度計 CNC_v3.2 PCB，
  使用 Seeed XIAO nRF52840 Sense 作為主控板。

  ⚠️ DEPRECATED VERSION:
  This code is no longer maintained.
  It compiles successfully, but runtime behavior is not guaranteed.

  開發板：Seeed XIAO nRF52840 Sense（請於 Arduino IDE 中選擇正確板型）
  適用裝置：刀把粗度計 CNC_v3.2 PCB

  ⚠️ 本版本程式碼已棄用，放在這裡僅為了未來參考與版本控制：
  僅保證可成功編譯，不保證實際執行功能正確。
*/

#include <Wire.h>
#include "src/RAK12029_LDC1614.h"  // Click to install library: http://librarymanager/All#RAK12029_LDC1614

#define RP            1000.0
#define INDUCTANCE    2.1
#define CAPATANCE     1.5
#define Q             17.4


RAK12029_LDC1614_Inductive ldc;

RAK12029_LDC1614_Inductive ldc1;
RAK12029_LDC1614_Inductive ldc2;


void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);


  time_t timeout = millis();
  Serial.begin(115200);
  while (!Serial)
  {
    if ((millis() - timeout) < 5000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }

  Serial.println("LDC 1614 TEST");
  delay(1000);

  ldc1.set_iic_addr(0x2A);
  delay(100);

  ldc1.LDC1614_init();

  if (ldc1.LDC1614_mutiple_channel_config(RP, INDUCTANCE, CAPATANCE, Q) != 0)
  {
    Serial.println("can't detect sensor!");
    while (1) delay(100);
  }


  ldc2.set_iic_addr(0x2B);
  delay(100);

  ldc2.LDC1614_init();

  if (ldc2.LDC1614_mutiple_channel_config(RP, INDUCTANCE, CAPATANCE, Q) != 0)
  {
    Serial.println("can't detect sensor!");
    while (1) delay(100);
  }

}

void loop()
{
  u16 value = 0;
  //ldc.IIC_read_16bit(LDC1614_READ_DEVICE_ID, &value);
  //original_read();

  tang_read(value);
  delay(10);

}

void tang_read(u16 value) {
  u32 Ldc1ChannelBuf[4] = {0};
  u32 Ldc2ChannelBuf[4] = {0};

  ldc1.LDC1614_get_channel_result(0, &Ldc1ChannelBuf[0]);
  ldc1.LDC1614_get_channel_result(1, &Ldc1ChannelBuf[1]);
  ldc1.LDC1614_get_channel_result(2, &Ldc1ChannelBuf[2]);
  ldc1.LDC1614_get_channel_result(3, &Ldc1ChannelBuf[3]);

  ldc2.LDC1614_get_channel_result(0, &Ldc2ChannelBuf[0]);
  ldc2.LDC1614_get_channel_result(1, &Ldc2ChannelBuf[1]);
  ldc2.LDC1614_get_channel_result(2, &Ldc2ChannelBuf[2]);
  ldc2.LDC1614_get_channel_result(3, &Ldc2ChannelBuf[3]);

  Serial.print(Ldc1ChannelBuf[0]);
  Serial.print("\t");
  Serial.print(Ldc1ChannelBuf[1]);
  Serial.print("\t");
  Serial.print(Ldc1ChannelBuf[2]);
  Serial.print("\t");
  Serial.print(Ldc1ChannelBuf[3]);
  Serial.print("\t");


  Serial.print(Ldc2ChannelBuf[0]);
  Serial.print("\t");
  Serial.print(Ldc2ChannelBuf[1]);
  Serial.print("\t");
  Serial.print(Ldc2ChannelBuf[2]);
  Serial.print("\t");
  Serial.println(Ldc2ChannelBuf[3]);

}


void original_read() {
  u16 value = 0;
  if (value == 0x3055) {
    u32 ChannelBuf[4] = {0};
    Serial.println("------------------------------------------------------------");
    for (uint8_t channelCount = 0; channelCount < 4; channelCount++)
    {
      /*sensor result value.you can make a lot of application according to its changes.*/
      //delay(100);
      if (ldc.LDC1614_get_channel_result(3 - channelCount, &ChannelBuf[channelCount]) == 0) //
      {
        if (0 != ChannelBuf[channelCount])
        {
          //Serial.printf("The result channel%d is:%d\r\n", channelCount + 1, ChannelBuf[channelCount]);
          Serial.println(ChannelBuf[channelCount]);
          digitalWrite(LED_BUILTIN, HIGH);
        }
      }
    }
    Serial.println("------------------------------------------------------------");
    Serial.println();
    //    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    //    delay(500);
  }
  else
  {
    Serial.println("The IIC communication fails,please reset the sensor!");
    delay(500);
  }
}
