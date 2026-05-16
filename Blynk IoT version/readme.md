# ESP32 Music Player - Blynk IoT & Web Hybrid Version

This project is a premium ESP32-based music player that can be controlled via **Blynk IoT** and a **Web Interface**. It features music-synchronized RGB lighting, an LCD display for lyrics, and an automated relay control system.

## 🛠 Hardware Pin Mapping (ESP32)

| Component | Pin | Description |
|---|---|---|
| **Buzzer** | GPIO 5 | Passive buzzer for music output |
| **I2C SDA** | GPIO 21 | LiquidCrystal I2C Data |
| **I2C SCL** | GPIO 22 | LiquidCrystal I2C Clock |
| **RGB LED (Red)** | GPIO 4 | PWM control for Red channel |
| **RGB LED (Green)**| GPIO 2 | PWM control for Green channel |
| **RGB LED (Blue)** | GPIO 13 | PWM control for Blue channel |
| **Relay** | GPIO 27 | Triggered after timer timeout |

---

## 📱 Blynk Virtual Pins (V-Pins) Definition

| Virtual Pin | Name | Datastream Type | Web Dashboard Widget | Description |
|---|---|---|---|---|
| **V0** | Song Selector | Integer | Slider / Dropdown | Select song from list (0-8) |
| **V1** | Stop All | Integer | Button | Stop music, timer, and reset relay |
| **V2** | Timer Slider | Integer | Slider | Set countdown time (1 - 3600 seconds) |
| **V3** | Start Timer | Integer | Button | Begin the timer countdown |
| **V4** | LCD Line 1 | String | Text Input | Input text for first line of LCD |
| **V5** | LCD Line 2 | String | Text Input | Input text for second line of LCD |
| **V6** | Update LCD | Integer | Button | Display V4/V5 text to the LCD |
| **V7** | Master LED | Integer | Switch | Power ON/OFF for RGB LED |
| **V8** | LED Sync | Integer | Switch | Toggle Music Sync (ON) vs Manual (OFF) |
| **V9** | ZeRGBa | String / RGB | ZeRGBa | Manual color picker |
| **V10** | Relay Toggle | Integer | Switch | Manual override Relay ON/OFF |
| **V11 - V19**| Hotkeys | Integer | Button | One-click play specific songs |
| **V20** | Status | String | Label / Labeled Value | Feedback from ESP32 |
| **V21** | Red Slider | Integer | Slider (0-255) | Manual Red (Alternative to ZeRGBa) |
| **V22** | Green Slider | Integer | Slider (0-255) | Manual Green (Alternative to ZeRGBa) |
| **V23** | Blue Slider | Integer | Slider (0-255) | Manual Blue (Alternative to ZeRGBa) |
| **V24** | Timer (+) | Integer | Button | Increment timer (+10s) |
| **V25** | Timer (-) | Integer | Button | Decrement timer (-10s) |

### 🎵 Daftar Lagu Hotkeys (V11 - V19)
Tombol ini digunakan untuk memutar lagu secara instan tanpa harus memilih lewat menu:
- **V11**: Fur Elise (Beethoven)
- **V12**: Happy Birthday
- **V13**: Its Not Like I Like You (Tsundere)
- **V14**: Mayor Minor (Scale Demo)
- **V15**: Nokia Classic Ringtone
- **V16**: Super Mario World 1-1
- **V17**: Sweet Bumblebee
- **V18**: Kiss Me Again
- **V19**: Tetris Theme (Korobeiniki)

---

## 📘 Tutorial Penambahan Pin (Datastream) di Blynk

Untuk menjalankan fitur ini, Anda harus mengatur **Datastream** di Blynk Console atau aplikasi Mobile:

1.  **Login** ke [Blynk.Console](https://blynk.cloud/).
2.  Buka menu **Templates** dan pilih template ESP32 Anda.
3.  Klik tab **Datastreams** -> **Edit**.
4.  Klik **+ New Datastream** -> **Virtual Pin**.
5.  Gunakan pengaturan berikut untuk setiap pin baru:
    *   **V21, V22, V23 (RGB Switches)**:
        *   Name: `Red Switch`, `Green Switch`, dsb.
        *   Data Type: `Integer`.
        *   Min: `0`, Max: `1`.
    *   **V24, V25 (Timer Adjust)**:
        *   Name: `Timer Plus`, `Timer Minus`.
        *   Data Type: `Integer`.
        *   Min: `0`, Max: `1` (Mode: Push Button).
6.  Klik **Save** dan **Save Template**.

---

## ✨ Fitur Utama

- **Dual Control**: Kontrol via Blynk App atau Browser (Web Dashboard).
- **RGB Manual Switch**: Anda bisa menyalakan R, G, dan B secara terpisah (V21-V23) untuk menghasilkan campuran warna (misal: R+G = Kuning).
- **Adjustable Timer**: Atur timer lewat slider (V2) atau tombol cepat (V24/V25).
- **Music Sync**: Lampu LED berkedip mengikuti nada musik secara otomatis.
- **Auto-Relay**: Relay akan menyala selama 3 detik setelah timer habis.

---

## 🚀 Cara Setup Cepat

1.  Buka `Esp32_Blynk.ino`.
2.  Ganti `BLYNK_AUTH_TOKEN`, `WIFI_SSID`, dan `WIFI_PASSWORD` sesuai milik Anda.
3.  Pastikan library `Blynk` dan `LiquidCrystal_I2C` sudah terinstall.
4.  Upload ke ESP32 dan nikmati musiknya!
