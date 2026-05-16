/*
 * ESP32 Hitung Mundur 00:15 → 00:00
 * Buzzer makin cepat, tepat 1 detik per langkah
 * SDA -> Pin 21 | SCL -> Pin 22 | Buzzer -> Pin 5
 */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BUZZER_PIN  5
#define SDA_PIN     21
#define SCL_PIN     22
#define LCD_ADDR    0x27
#define LCD_COLS    16
#define LCD_ROWS    2

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

#define C5  523
#define E5  659
#define G5  784
#define C6  1047
#define E6  1319

// Interval antar beep per detik (ms), indeks 0 = detik 15, indeks 14 = detik 1
int beepInterval[] = {
  900,  // detik 15
  850,  // detik 14
  800,  // detik 13
  720,  // detik 12
  640,  // detik 11
  560,  // detik 10
  480,  // detik 9
  400,  // detik 8
  330,  // detik 7
  270,  // detik 6
  220,  // detik 5
  180,  // detik 4
  150,  // detik 3
  120,  // detik 2
  100   // detik 1
};

void tampilLCD(int detik) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Hitung Mundur ");
  lcd.setCursor(0, 1);
  lcd.print("     00:");
  if (detik < 10) lcd.print("0");
  lcd.print(detik);
  lcd.print("        ");
}

// Beep tepat selama 1000ms menggunakan millis()
void beepDetik(int detikSisa) {
  int idx      = 15 - detikSisa;  // 0 = detik 15, 14 = detik 1
  int interval = beepInterval[idx];
  int beepLama = 50;
  int freq     = 880 + (idx * 80);
  if (freq > 2000) freq = 2000;

  unsigned long mulai = millis();

  while (millis() - mulai < 1000) {
    unsigned long sisaDetik = 1000 - (millis() - mulai);

    // Pastikan masih cukup waktu untuk 1 beep penuh
    if (sisaDetik < (unsigned long)beepLama) break;

    tone(BUZZER_PIN, freq, beepLama);
    delay(beepLama);
    noTone(BUZZER_PIN);

    // Jeda, tapi tidak melebihi sisa waktu detik ini
    unsigned long jedaAktual = interval - beepLama;
    unsigned long sisaSetelahBeep = 1000 - (millis() - mulai);
    if (jedaAktual > sisaSetelahBeep) break;
    delay(jedaAktual);
  }
}

void nadaAkhir() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Hitung Mundur ");
  lcd.setCursor(0, 1);
  lcd.print("     00:00      ");

  delay(300);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" ---Time out---");
  lcd.setCursor(0, 1);
  lcd.print("");

  int nada[] = { C5, E5, G5, C6, E6, C6, E6 };
  int lama[] = { 120, 120, 120, 200, 400, 150, 700 };

  for (int i = 0; i < 7; i++) {
    tone(BUZZER_PIN, nada[i], lama[i]);
    delay(lama[i] + 40);
    noTone(BUZZER_PIN);
    delay(20);
  }
}
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);

  // Layar pembuka
  lcd.setCursor(0, 0);
  lcd.print("  Hitung Mundur ");
  lcd.setCursor(0, 1);
  lcd.print(" 00:15 -> 00:00");
  delay(1500);

  // Hitung mundur 15 → 1
  for (int detik = 15; detik >= 1; detik--) {
    tampilLCD(detik);
    beepDetik(detik);
  }

  // Tampil 00:00 + nada akhir
  nadaAkhir();
}

void loop() {
  // Selesai
}
