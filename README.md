# CNC Toolholder Roughness Gauge
刀把粗度計感測系統（多版本程式碼）

本專案為 **CNC Toolholder Roughness Gauge 刀把粗度量測系統** 的韌體程式集，依照通訊方式與功能演進分為多個版本：

- ✅ **CNC_v4**：目前主力維護版本，透過 USB 有線通訊將 LDC1614 感測資料傳輸至主機
- ✅ **CNC_BLE_v1**：無線版本，利用 Bluetooth Low Energy（BLE）廣播方式傳送感測值
- ⚠️ **CNC_v1 ~ v3**：早期開發版本，已停止維護（Deprecated）

---

This repository contains firmware for the **CNC Toolholder Roughness Gauge** system. It includes multiple versions developed over time, categorized by their communication method:

- ✅ **CNC_v4**: Latest stable version using USB for data transmission (wired)
- ✅ **CNC_BLE_v1**: BLE-based version broadcasting sensor data wirelessly
- ⚠️ **CNC_v1 to v3**: Legacy versions that are no longer maintained (deprecated)

---

## 🧰 Requirements | 使用環境

- ✅ Arduino IDE 1.8.x / 2.x
- ✅ Seeeduino XIAO nRF52840 Sense
- ✅ LDC1614 Inductance-to-Digital Converter (via I2C)
- ✅ Optional: Bluetooth/BLE support via Bluefruit (for BLE versions)

---

## 📦 Features | 功能特色

- 多通道 LDC1614 感測值讀取（支援 ch0～ch3）
- 零點偏移校正、自動平均濾波
- 可選擇 USB 串列輸出或 BLE 廣播傳輸
- 支援非阻塞定時與多任務排程

---

## 🚀 Getting Started | 快速開始

1. 安裝 Arduino IDE 並選擇 **Seeeduino XIAO nRF52840 Sense**
2. 將原始碼下載至本地，打開對應 `.ino` 檔案
3. 根據使用版本調整連線與功能（USB 或 BLE）
4. 編譯與上傳，即可開始使用

---

## ℹ️ Notes | 附註

- BLE 模式需搭配具備藍牙接收功能之裝置（如手機 App 或藍牙 sniffer）  
- LDC1614 為高精度感測器，其最大資料更新頻率受限於其內部轉換時間（約 13～18Hz）  
- 程式中使用 `UTL_Timer` 與 `UTL_Event` 實作非阻塞時間控制

---

## 👨‍💻 Author & License

- Author: [Long Jun, Chiou](https://github.com/Long-Jun) @ NTUT UTL & SPIE Lab  
- Co-Author: Chun Ru, Tang  
- License: Apache-2.0

---

## 💬 Feedback | 問題與建議

若您在使用過程中有任何問題或改進建議，歡迎提出 [Issue](https://github.com/Long-Jun/CNC_Tool_Holder/issues) 或提交 Pull Request！

歡迎研究合作與教學應用引用本專案。
