/*
 * ESP32 Music Player dengan LCD I2C
 * Tetris Theme - Korobeiniki (Russian Folk Song)
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
#define BPM          144
#define BEAT         (60000 / BPM)       // 1 ketuk = 416ms
#define Q            BEAT                // Quarter note
#define H            (BEAT * 2)          // Half note
#define E            (BEAT / 2)          // Eighth note
#define DQ           (BEAT * 3 / 2)      // Dotted quarter

// === Frekuensi nada (Hz) ===
#define E4   330
#define F4   349
#define G4   392
#define A4   440
#define B4   494
#define C5   523
#define D5   587
#define E5   659
#define F5   698
#define G5   784
#define A5   880
#define AS4  466
#define DS5  622
#define DIAM   0

// =========================================================
// === TETRIS THEME - Korobeiniki =========================
// =========================================================
const int totalNada = 40;

int melodi[] = {
  // Baris A (melodi utama)
  E5, B4, C5, D5, C5, B4,
  A4, A4, C5, E5, D5, C5,
  B4, C5, D5, E5,
  C5, A4, A4,

  // Baris B (jembatan)
  D5, F5, A5, G5, F5,
  E5, C5, E5, D5, C5,
  B4, B4, C5, D5, E5,
  C5, A4, A4, DIAM,

  // Penutup tegas
  E5, E5
};

int durasi[] = {
  // Baris A
  Q,  E,  E,  Q,  E,  E,
  Q,  E,  E,  Q,  E,  E,
  DQ, E,  Q,  Q,
  Q,  Q,  H,

  // Baris B
  Q,  E,  Q,  E,  E,
  DQ, E,  Q,  E,  E,
  Q,  E,  E,  Q,  Q,
  Q,  Q,  H,  E,

  // Penutup
  Q, H
};

// Baris 1 LCD
const char* teks1[] = {
  // Baris A
  "Tetris!!", "da~", "da~", "TETRIS!", "da~", "da~",
  "Block!!", "da~", "da~", "Drop it!", "da~", "da~",
  "Line~~", "da~", "Clear!!", "YEAH!!",
  "da da~", "Score!!", "TETRIS!!",

  // Baris B
  "Level UP!", "da~", "Faster!!", "da~", "da~",
  "T-Spin!!", "da~", "Combo!!", "da~", "da~",
  "4 Lines!", "da~", "da~", "TETRIS!!", "CLEAR!!",
  "da da~", "TOP!!", "SCORE!!", "...",

  // Penutup
  "GAME", "OVER!!"
};

// Baris 2 LCD
const char* teks2[] = {
  // Baris A
  "~da da da~", "~", "~", "~da da da~", "~", "~",
  "~drop!~", "~", "~", "~line!~", "~", "~",
  "~clear!~", "~", "~yeah!~", "(*^o^*)",
  "~da da~", "~score!~", "(*^_^*)/",

  // Baris B
  "~level up!~", "~", "~faster!~", "~", "~",
  "~t-spin!~", "~", "~combo!~", "~", "~",
  "~tetris!~", "~", "~", "~clear!~", "(>o<)/",
  "~da da~", "~top!~", "~score!~", "~",

  // Penutup
  "~dun dun~", "~DUN DUN!!~"
};

// =========================================================
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

void mainkanNada(int frekuensi, int lama) {
  int mainLama = lama * 0.85; // 85% bunyi, 15% jeda antar nada
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
  lcd.print("Tetris Theme    ");
  lcd.setCursor(0, 1);
  lcd.print("Korobeiniki :D  ");
  delay(1800);

  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);

  for (int i = 0; i < totalNada; i++) {
    tampilkanLCD(teks1[i], teks2[i]);
    mainkanNada(melodi[i], durasi[i]);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GAME CLEAR! :D  ");
  lcd.setCursor(0, 1);
  lcd.print("EN untuk ulangi ");
}

void loop() {
  // Tekan BOOT untuk mengulang
}