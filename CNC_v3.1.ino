#include "RAK12029_LDC1614.h"  // Click to install library: http://librarymanager/All#RAK12029_LDC1614
#include "math.h"
#include "LSM6DS3.h"
#include "Wire.h"
#include <Timer.h>

// millis()     : 系統執行時間（毫秒）
// LDC_Mode     : LDC1614 模式（0=單通道, 1=雙通道, 2=四通道）
// TMP275_temp  : TMP275 溫度感測器讀值（°C, 12-bit 精度）
// LDC_ch0      : LDC1614 第 0 通道原始讀值
// LDC_ch1      : LDC1614 第 1 通道原始讀值
// AccX         : IMU 加速度 X 軸（單位 g）
// AccY         : IMU 加速度 Y 軸（單位 g）
// AccZ         : IMU 加速度 Z 軸（單位 g）
// GyroX        : IMU 角速度 X 軸（單位 °/s）
// GyroY        : IMU 角速度 Y 軸（單位 °/s）
// GyroZ        : IMU 角速度 Z 軸（單位 °/s）
// IMU_Temp     : IMU 內建溫度感測器（°C）

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



// Mode, 0=>1ch, 1=>2ch, 2=>4ch
int8_t LDC_Mode;

uint8_t LDC_ch = 0;
float PCB_Temp;
uint32_t LdcChannelBuf[4];

float Acc[3];
float Gyro[3];
float Thermal;

RAK12029_LDC1614_Inductive ldc;

//Create a instance of class LSM6DS3
LSM6DS3 IMU6D(I2C_MODE, 0x6A);

Timer Print_test;
Timer Print_USB;
Timer TMP275;
Timer LDC;
Timer IMU;

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
  while (!Serial) delay(10);

  Wire.begin();
  Wire.setClock(100000);
  delayMicroseconds(10000);

  tmp275_init();

  IMU6D.begin();

  ldc.set_iic_addr(0x2A);
  ldc.LDC1614_init();
  ldc.LDC1614_twise_channel_config(RP, INDUCTANCE, CAPATANCE, Q);

  Print_USB.every(20, print_value);
  Print_test.every(20, print_test);
  TMP275.every(10, read_pcb_temp);
  LDC.every(10, tang_two_read);
  IMU.every(20, read_imu);

  Serial.println("LDC 1614 TEST");
  delay(2000);

}

void loop() {
  Print_USB.update();
  //Print_test.update();
  TMP275.update();
  LDC.update();
  IMU.update();
}

void tang_single_read() {
  LDC_Mode = 0;
  ldc.LDC1614_get_channel_result(LDC_ch, &LdcChannelBuf[0]);
}

void tang_two_read() {
  LDC_Mode = 1;
  ldc.LDC1614_get_channel_result(0, &LdcChannelBuf[0]);
  ldc.LDC1614_get_channel_result(1, &LdcChannelBuf[1]);
}

void tang_multi_read() {
  LDC_Mode = 2;
  ldc.LDC1614_get_channel_result(0, &LdcChannelBuf[0]);
  ldc.LDC1614_get_channel_result(1, &LdcChannelBuf[1]);
  ldc.LDC1614_get_channel_result(2, &LdcChannelBuf[2]);
  ldc.LDC1614_get_channel_result(3, &LdcChannelBuf[3]);
}

void print_value() {
  int32_t test_coil;
  Serial.print(millis());
  Serial.print(",");
  Serial.print(LDC_Mode);
  Serial.print(",");
  Serial.print(PCB_Temp, 4);
  Serial.print(",");

  switch (LDC_Mode) {
    case 0:
      Serial.print(LdcChannelBuf[0]);
      Serial.print(",");
      break;
    case 1:
      Serial.print(LdcChannelBuf[0]);
      Serial.print(",");
      Serial.print(LdcChannelBuf[1]);
      Serial.print(",");
      test_coil = LdcChannelBuf[0] - LdcChannelBuf[1];
      Serial.print(test_coil);
      Serial.print(",");
      break;
    case 2:
      Serial.print(LdcChannelBuf[0]);
      Serial.print(",");
      Serial.print(LdcChannelBuf[1]);
      Serial.print(",");
      Serial.print(LdcChannelBuf[2]);
      Serial.print(",");
      Serial.print(LdcChannelBuf[3]);
      Serial.print(",");
      break;
  }

  Serial.print(Acc[0], 4);
  Serial.print(",");
  Serial.print(Acc[1], 4);
  Serial.print(",");
  Serial.print(Acc[2], 4);
  Serial.print(",");
  Serial.print(Gyro[0], 4);
  Serial.print(",");
  Serial.print(Gyro[1], 4);
  Serial.print(",");
  Serial.print(Gyro[2], 4);
  Serial.print(",");
  Serial.print(Thermal, 4);
  Serial.println();
}

void print_test() {
  int32_t test_coil;

  //Serial.println(LdcChannelBuf[0]);

  test_coil = LdcChannelBuf[0] - LdcChannelBuf[1];
  Serial.println(test_coil);
}
// TMP275初始化
void tmp275_init() {
  Wire.beginTransmission(TMP275_ADDR);
  Wire.write(CONFIG_REG);  // 0x01
  Wire.write(0x60);        // MSB（不變）
  Wire.write(0x03);        // ✅ 設定為 12-bit + SD=0（自動轉換）
  Wire.endTransmission();
  delayMicroseconds(10000);

  Wire.beginTransmission(TMP275_ADDR);
  Wire.write(CONFIG_REG);
  Wire.endTransmission(false);

  Wire.requestFrom(TMP275_ADDR, 2);
  if (Wire.available() < 2)
    Serial.println("Failed to read TMP275 config!");
}


// Read TMP275 onboard TEMP sensor
void read_pcb_temp() {
  Wire.beginTransmission(TMP275_ADDR);
  Wire.write(TEMP_REG);  // = 0x00
  Wire.endTransmission(false);

  Wire.requestFrom(TMP275_ADDR, 2);
  if (Wire.available() >= 2) {
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    int16_t raw = ((msb << 8) | lsb) >> 4;
    if (raw & 0x800) raw |= 0xF000; // 補正負數
    PCB_Temp = raw * 0.0625;
  }
}

void read_imu() {
  Acc[0] = IMU6D.readFloatAccelX();
  Acc[1] = IMU6D.readFloatAccelY();
  Acc[2] = IMU6D.readFloatAccelZ();
  Gyro[0] = IMU6D.readFloatGyroX();
  Gyro[1] = IMU6D.readFloatGyroY();
  Gyro[2] = IMU6D.readFloatGyroZ();
  Thermal = IMU6D.readTempC();
}


///////////////////////////////////////
/////////      棄用程式區      //////////
///////////////////////////////////////
/*

  void original_read() {
  u16 value = 0;
  if (value == 0x3055) {
    u32 ChannelBuf[4] = {0};
    Serial.println("------------------------------------------------------------");
    for (uint8_t channelCount = 0; channelCount < 4; channelCount++)
    {
      // sensor result value.you can make a lot of application according to its changes.
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

*/
