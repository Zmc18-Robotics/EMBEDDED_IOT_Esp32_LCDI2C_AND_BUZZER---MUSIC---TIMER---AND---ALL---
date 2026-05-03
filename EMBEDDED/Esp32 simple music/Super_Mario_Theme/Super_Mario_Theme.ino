/*
 * ESP32 Music Player dengan LCD I2C
 * Super Mario Bros. Theme — Nintendo (1985)
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
#define C6  1047
#define DIAM   0

// =========================================================
// === SUPER MARIO BROS. THEME (Main Theme) ================
// =========================================================

const int totalNada = 40;

int melodi[] = {
  E5, E5, DIAM, E5, DIAM, C5, E5, DIAM,
  G5, DIAM, DIAM, DIAM,
  G4, DIAM, DIAM, DIAM,
  C5, DIAM, DIAM, G4, DIAM, DIAM,
  E4, DIAM, DIAM, A4, DIAM, B4, DIAM,
  A4, G4, E5, DIAM, G5,
  A5, DIAM, F5, G5, DIAM
};

int durasi[] = {
  150, 150, 150, 150, 150, 150, 150, 150,   // frasa 1
  300, 150, 150, 300,                         // nada puncak G5
  300, 150, 150, 300,                         // nada G4 rendah
  300, 150, 300, 300, 150, 300,               // frasa tengah
  150, 150, 300, 150, 150, 150, 150,          // frasa A-B
  200, 200, 200, 200, 200,                    // frasa naik
  200, 150, 150, 200, 300                     // penutup
};

// Baris 1 LCD
const char* teks1[] = {
  "Super Mario!", "Super Mario!", "...", "Super Mario!",
  "...", "Go Mario!", "Go Mario!", "...",
  "World 1-1", "...", "...", "World 1-1",
  "Goomba!", "...", "...", "Jump!",
  "1UP!", "...", "Coins:", "Coins:x3",
  "...", "Coins:x5", "Run!", "...",
  "...", "Princess!", "...", "Save her!",
  "...", "Wahoo!", "YA-HOO!", "...",
  "Mario!!", "MARIO!!", "...", "YEAH!!",
  "...", "Its-a me", "Mario!!!", "WINNER!"
};

// Baris 2 LCD
const char* teks2[] = {
  "da da~", "da DUM!", "~", "da da da~",
  "~", "DUM DUM!", "DUM!", "~",
  "~coin~", "~", "~", "da da DUM!",
  "stomp!", "~", "~", "Wheee~",
  "~", "~", "*bing!*", "*bing bing!*",
  "~", "*bing!*", "da da da~", "~",
  "~", "da da DUM!", "~", "da da~",
  "~", "WA-HOO!", "(*^o^*)", "~",
  "da DUM DUM~", "da da DUM!!", "~", "(*^_^*)/",
  "~", "~", "(^o^)/", "GAME CLEAR!"
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
  delay(40);
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
  lcd.print("Super Mario Bros");
  lcd.setCursor(0, 1);
  lcd.print("Nintendo 1985 :)");
  delay(1800);

  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);

  for (int i = 0; i < totalNada; i++) {
    tampilkanLCD(teks1[i], teks2[i]);
    mainkanNada(melodi[i], durasi[i]);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GAME CLEAR! :D");
  lcd.setCursor(0, 1);
  lcd.print("EN untuk ulangi");
}

void loop() {
  // Tekan BOOT untuk mengulang
}