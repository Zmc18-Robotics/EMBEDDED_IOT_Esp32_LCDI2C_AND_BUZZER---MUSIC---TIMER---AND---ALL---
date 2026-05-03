/*
 * ESP32 Music Player dengan LCD I2C
 * Fur Elise - Ludwig van Beethoven (1810)
 * Notasi terverifikasi dari Arduino community
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

// === Tempo ===
#define BPM          120
#define BEAT         (60000 / BPM)
#define Q            BEAT
#define H            (BEAT * 2)
#define E            (BEAT / 2)
#define S            (BEAT / 4)
#define DQ           (BEAT * 3 / 2)

// === Frekuensi nada (Hz) — standar internasional ===
#define E4   330
#define DS4  311
#define D4   294
#define CS4  277
#define C3   131
#define C4   262
#define B3   247
#define AS3  233
#define A3   220
#define GS3  208
#define G3   196
#define E3   165
#define A4   440
#define C5   523
#define D5   587
#define E5   659
#define F4   349
#define G4   392
#define DIAM   0

// =========================================================
// === FUR ELISE - Beethoven (notasi baku Arduino) =========
// =========================================================
const int totalNada = 44;

int melodi[] = {
  // Motif utama (diulang 2x)
  E4, DS4, E4, DS4, E4, B3, D4, C4,
  A3, DIAM, C3, E3, A3,
  B3, DIAM, E3, GS3, B3,
  C4, DIAM, E3, E4, DS4,

  // Motif utama lagi
  E4, DS4, E4, B3, D4, C4,
  A3, DIAM, C3, E3, A3,
  B3, DIAM, E3, C4, B3,
  A3, DIAM, DIAM, DIAM,

  // Penutup
  E4, DS4, E4, DS4
};

int durasi[] = {
  // Motif utama
  S, S, S, S, S, S, S, S,
  Q, E, S, S, S,
  Q, E, S, S, S,
  Q, E, S, S, S,

  // Motif utama lagi
  S, S, S, S, S, S,
  Q, E, S, S, S,
  Q, E, S, S, S,
  Q, E, E, E,

  // Penutup
  S, S, S, S
};

// Baris 1 LCD
const char* teks1[] = {
  // Motif utama
  "Fur Elise~", "da~", "da~", "da~", "Beethoven", "da~", "da~", "da~",
  "1810~~", "...", "da~", "da~", "da~",
  "Piano~~", "...", "da~", "da~", "da~",
  "Classic~", "...", "da~", "Fur~", "da~",

  // Motif ulang
  "Elise~~", "da~", "da~", "da~", "da~", "da~",
  "Ludwig~", "...", "da~", "da~", "da~",
  "van~~", "...", "da~", "Beethoven", "da~",
  "1810~~", "...", "...", "...",

  // Penutup
  "Fur~", "Elise~", "da~", "da~"
};

// Baris 2 LCD
const char* teks2[] = {
  // Motif utama
  "~da da da~", "~", "~", "~", "~da da~", "~", "~", "~",
  "~1810~", "~", "~", "~", "~",
  "~piano~", "~", "~", "~", "~",
  "~classic~", "~", "~", "~fur~", "~",

  // Motif ulang
  "~elise~", "~", "~", "~", "~", "~",
  "~ludwig~", "~", "~", "~", "~",
  "~van~", "~", "~", "~beethoven~", "~",
  "(*^_^*)", "~", "~", "~",

  // Penutup
  "~fur~", "~elise~", "~da~", "~da~"
};

// =========================================================
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

void mainkanNada(int frekuensi, int lama) {
  int mainLama = lama * 0.85;
  int jedaNada  = lama - mainLama;

  if (frekuensi == 0) {
    noTone(BUZZER_PIN);
    delay(lama);
  } else {
    tone(BUZZER_PIN, frekuensi, mainLama);
    delay(mainLama);
    noTone(BUZZER_PIN);
    delay(jedaNada);
  }
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
  lcd.print("Fur Elise       ");
  lcd.setCursor(0, 1);
  lcd.print("Beethoven 1810  ");
  delay(1800);

  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);

  for (int i = 0; i < totalNada; i++) {
    tampilkanLCD(teks1[i], teks2[i]);
    mainkanNada(melodi[i], durasi[i]);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fur Elise :D    ");
  lcd.setCursor(0, 1);
  lcd.print("EN untuk ulangi ");
}

void loop() {
  // Tekan BOOT untuk mengulang
}