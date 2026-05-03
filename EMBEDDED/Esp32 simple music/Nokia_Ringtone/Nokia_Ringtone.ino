/*
 * ESP32 Music Player dengan LCD I2C
 * Nokia Ringtone (Gran Vals — Francisco Tárrega)
 * SDA -> Pin 21 | SCL -> Pin 22 | Buzzer -> Pin 5
 * Library: LiquidCrystal_I2C by Frank de Brabander
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BUZZER_PIN   5
#define SDA_PIN      21
#define SCL_PIN      22
#define LCD_ADDR     0x27
#define LCD_COLS     16
#define LCD_ROWS     2

// === Oktaf 0 - Mayor (rendah) ===
#define DO      262
#define RE      294
#define MI      330
#define FA      349
#define SOL     392
#define LA      440
#define SI      494

// === Oktaf 1 - Mayor (tengah) ===
#define DO1     523
#define RE1     587
#define MI1     659
#define FA1     698
#define SOL1    784
#define LA1     880
#define SI1     988

// === Oktaf 2 - Mayor (tinggi) ===
#define DO2     1047
#define RE2     1175
#define MI2     1319
#define FA2     1397
#define SOL2    1568
#define LA2     1760
#define SI2     1976

// === Oktaf 1 - Minor (tengah) ===
#define MIm1    622
#define REm1    622
#define LAm1    831
#define SOLm1   831

// === Diam / Jeda ===
#define DIAM    0

// =========================================================
// === NOKIA RINGTONE (Gran Vals — 4 frasa, 32 nada) =======
// =========================================================

const int totalNada = 32;

// Melodi Nokia Ringtone yang ikonik
// Pola: E5 D5 | F#4 G#4 | C#5 B4 | E4 F#4 |
//       B4 A4 | C#5 E5 | A5... | (repeat)
// Menggunakan frekuensi terdekat dari define di atas
int melodi[] = {
  MI2, RE2, FA1, LA1,              // "da da da DUM"
  DO2, SI1, MI1, SOL1,             // "da da da DUM"
  LA1, SOL1, DO2, MI2,             // "da da da DUM"
  LA2, DIAM,                       // nada puncak + jeda
  MI2, RE2, FA1, LA1,              // repeat frasa 1
  DO2, SI1, MI1, SOL1,             // repeat frasa 2
  LA1, SOL1, DO2, MI2,             // repeat frasa 3
  LA2, SOL2, MI2, DO2,             // turun menuju akhir
  DIAM                             // jeda akhir
};

int durasi[] = {
  // Frasa 1: "da da da DUM"
  200, 200, 150, 400,
  // Frasa 2: "da da da DUM"
  200, 200, 150, 400,
  // Frasa 3: "da da da DUM"
  200, 200, 150, 400,
  // Nada puncak + jeda antar bagian
  500, 600,
  // Frasa 4 (repeat 1)
  200, 200, 150, 400,
  // Frasa 5 (repeat 2)
  200, 200, 150, 400,
  // Frasa 6 (repeat 3)
  200, 200, 150, 400,
  // Resolusi turun
  300, 250, 250, 400,
  // Jeda akhir
  1000
};

// Baris 1 LCD: nama / aksi
const char* teks1[] = {
  // Frasa 1
  "Nokia...", "Nokia..", "Nokia.", "Nokia!",
  // Frasa 2
  "Gran Vals", "Gran Vals~", "~", "Nokia!",
  // Frasa 3
  "Francisco", "Tarrega", "1902", "Nokia!",
  // Puncak
  "** NOKIA **", "",
  // Repeat 1
  "Nokia...", "Nokia..", "Nokia.", "Nokia!",
  // Repeat 2
  "Calling...", "Calling..", "Calling.", "INCOMING!",
  // Repeat 3
  "Ring ring~", "Ring ring!", "~Nokia~", "NOKIA!!!",
  // Resolusi
  "Gotta pick", "up that", "classic", "ringtone~",
  // Akhir
  ""
};

// Baris 2 LCD: konteks / efek
const char* teks2[] = {
  // Frasa 1
  "da...", "da da..", "da da da..", "DUM!",
  // Frasa 2
  "da...", "da da..", "da da da..", "DUM!",
  // Frasa 3
  "da...", "da da..", "da da da..", "DUM!",
  // Puncak
  "~3310 vibes~", "",
  // Repeat 1
  "da...", "da da..", "da da da..", "DUM!",
  // Repeat 2
  "brr...", "brrring..", "RRIING!!", ">phonepickup<",
  // Repeat 3
  "~", "~*~", "~*~*~", "(*.*)/",
  // Resolusi
  "from 2003", "flip open", "hello?", "(^_^)/",
  // Akhir
  ""
};

// =========================================================

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

void mainkanNada(int frekuensi, int lama) {
  if (frekuensi == 0) {
    noTone(BUZZER_PIN);
    delay(lama);
  } else {
    tone(BUZZER_PIN, frekuensi, lama);
    delay(lama);
    noTone(BUZZER_PIN);
  }
  delay(50);
}

void tampilkanLCD(const char* baris1, const char* baris2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(baris1);
  lcd.setCursor(0, 1);
  lcd.print(baris2);
}

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nokia Ringtone");
  lcd.setCursor(0, 1);
  lcd.print("Gran Vals 1902");
  delay(1500);

  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);

  for (int i = 0; i < totalNada; i++) {
    tampilkanLCD(teks1[i], teks2[i]);
    mainkanNada(melodi[i], durasi[i]);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Selesai!");
  lcd.setCursor(0, 1);
  lcd.print("EN untuk ulangi");
}

void loop() {
  // Tekan BOOT untuk mengulang
}