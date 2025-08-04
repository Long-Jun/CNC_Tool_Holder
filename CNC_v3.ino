#include "RAK12029_LDC1614.h"  // Click to install library: http://librarymanager/All#RAK12029_LDC1614
#include "math.h"
#include <Wire.h>
#include <Timer.h>

#define RP 0.631
#define INDUCTANCE 1.5458
#define CAPATANCE 1000.0
#define Q 16.04

#define TAMP_ALERT 6
#define LDC_INT 7
#define LDC_SD 8

int LDC_ch = 0;

RAK12029_LDC1614_Inductive ldc;

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

  time_t timeout = millis();
  Serial.begin(115200);
  while (!Serial) delay(10);

  Wire.begin();
  Wire.setClock(400000);
  delayMicroseconds(10000);

  Serial.println("LDC 1614 TEST");

  ldc.set_iic_addr(0x2A);
  delayMicroseconds(10000);

  ldc.LDC1614_init();



  //  if (ldc.LDC1614_mutiple_channel_config(RP, INDUCTANCE, CAPATANCE, Q) != 0)
  //  {
  //    Serial.println("can't detect sensor!");
  //    while (1) delay(100);
  //  }
  //}

  if (ldc.LDC1614_single_channel_config(LDC_ch, RP, INDUCTANCE, CAPATANCE, Q) != 0)
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

  tang_single_read();
  //  tang_two_read();
  //  tang_multi_read();
  delay(10);

}

void tang_single_read() {
  u32 LdcChannelBuf = 0;

  ldc.LDC1614_get_channel_result(LDC_ch, &LdcChannelBuf);

  Serial.print(LdcChannelBuf);
  Serial.println();
}

void tang_two_read() {
  u32 LdcChannelBuf[2] = {0};

  ldc.LDC1614_get_channel_result(0, &LdcChannelBuf[0]);
  ldc.LDC1614_get_channel_result(1, &LdcChannelBuf[1]);

  Serial.print(LdcChannelBuf[0]);
  Serial.print("\t");
  Serial.print(LdcChannelBuf[1]);
  Serial.println();
}

void tang_multi_read() {
  u32 LdcChannelBuf[4] = {0};

  ldc.LDC1614_get_channel_result(0, &LdcChannelBuf[0]);
  ldc.LDC1614_get_channel_result(1, &LdcChannelBuf[1]);
  ldc.LDC1614_get_channel_result(2, &LdcChannelBuf[2]);
  ldc.LDC1614_get_channel_result(3, &LdcChannelBuf[3]);

  Serial.print(LdcChannelBuf[0]);
  Serial.print("\t");
  Serial.print(LdcChannelBuf[1]);
  Serial.print("\t");
  Serial.print(LdcChannelBuf[2]);
  Serial.print("\t");
  Serial.print(LdcChannelBuf[3]);
  Serial.println();
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
