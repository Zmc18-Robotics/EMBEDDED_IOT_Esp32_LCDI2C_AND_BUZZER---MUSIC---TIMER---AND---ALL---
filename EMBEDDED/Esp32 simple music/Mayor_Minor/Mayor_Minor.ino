/*
 * ESP32 Music Player dengan LCD I2C
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

// === Oktaf 0 - Minor (rendah) ===
#define DOm     277
#define REm     311
#define MIm     311
#define FAm     370
#define SOLm    415
#define LAm     415
#define SIm     466

// === Oktaf 1 - Minor (tengah) ===
#define DOm1    554
#define REm1    622
#define MIm1    622
#define FAm1    740
#define SOLm1   831
#define LAm1    831
#define SIm1    932

// === Oktaf 2 - Minor (tinggi) ===
#define DOm2    1109
#define REm2    1245
#define MIm2    1245
#define FAm2    1480
#define SOLm2   1661
#define LAm2    1661
#define SIm2    1865

// === Diam / Jeda ===
#define DIAM    0

// =========================================================
// === EDIT BAGIAN INI =====================================
// =========================================================

const int totalNada = 6;

int melodi[] = {
  DO2,
  RE2,
  MI2,
  MIm2,
  REm2,
  DOm2
};

int durasi[] = {
  400,
  400,
  400,
  400,
  400,
  600
};

const char* teks1[] = {
  "Do Mayor",
  "Re Mayor",
  "Mi Mayor",
  "Mi Minor",
  "Re Minor",
  "Do Minor"
};

const char* teks2[] = {
  "Oktaf 2",
  "Oktaf 2",
  "Oktaf 2",
  "Oktaf 2",
  "Oktaf 2",
  "Oktaf 2"
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
  lcd.print("ESP32 Music");
  lcd.setCursor(0, 1);
  lcd.print("Siap...");
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