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

// === Oktaf 3 - Mayor (sangat tinggi) ===
#define DO3     2093
#define RE3     2349
#define MI3     2637
#define FA3     2794
#define SOL3    3136
#define LA3     3520
#define SI3     3951

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

// === Oktaf 3 - Minor (sangat tinggi) ===
#define DOm3    2217
#define REm3    2489
#define MIm3    2489
#define FAm3    2960
#define SOLm3   3322
#define LAm3    3322
#define SIm3    3729

// === Diam / Jeda ===
#define DIAM    0

// =========================================================
// === EDIT BAGIAN INI =====================================
// =========================================================

const int totalNada = 49;

int melodi[] = {
  SOL1, SOL1, SOL1, MI2, RE2, DO2, DO2, LA1, DO2, DIAM,
  LA2, SOL2, FA2, SOL2, LA2,
  MI2, RE2, DO2, RE2, DO2,
  SOLm2, SOLm2, SOLm2, SOLm2, SOL2, SOLm2, MI3, RE3, DO3, DO3, RE3, LA2,
  LA2, SOL2, FA2, SOL2, LA2,
  MI2, RE2, DO2, RE2, DO2,
  SOL2, SOL2, MI3, DO3, RE2, DO2,
  DIAM
};

// Total nada tetap 49
int durasi[] = {
  // "You are not My Prince in this!" (Bagian tegas/marah)
  250, 250, 250, 300, 200, 250, 350, 150, 400, 1500, // DIAM lebih singkat agar tidak terlalu lama kosong

  // "I... I am not a DERE!" (Mulai gugup, agak cepat)
  400, 200, 200, 200, 500, 

  // "a Tsun... >///< ...a Tsun-Dere"
  400, 200, 200, 200, 500,

  // "I Have a jello HEART!" (Bagian ini biasanya sangat cepat di lagu aslinya)
  150, 150, 150, 150, 150, 300, 600, 200, 200, 200, 200, 400,

  // "I'm not blush- BLUSHING!" (Gugup parah)
  150, 150, 150, 150, 600,

  // "I was just rush- RUSHING!"
  250, 150, 150, 150, 600,

  // "andddd for- forgotttt the Sun- SUNSCREEN!" (Melambat di akhir untuk efek dramatis)
  800, 400, 1000, 200, 200, 600,
  
  1000 // DIAM Akhir
};

const char* teks1[] = {
  "You are not My", "You are not My", "You are not My", "You are not My", "You are not My", "You are not My", 
  "You are not My", "You are not My", "You are not My", "",
  "I", "I am", "I am not", "I am not a", "I am not a DERE!",
  "a Tsun", ">///<", "a Tsun", "a Tsun-Dere", ">///<",
  "I", "I Have", "I Have a", "I Have a", "I Have a jel-", "I Have a jello", "HEART!", "I'm", "I'm not", "I'm not that", "I'am not that","I'am not that",
  "I'm", "I'm", "I'am not", "I'am not blush-", ">///< >:(",
  "I", "I was", "I was just", "I was just rush-", "RUSHING!!!",
  "andddd", "for-", "forgotttt", "the", "Sun-", "SUNSCREEN!",
  ""
};

const char* teks2[] = {
  "Prince in this!", "Prince in this!", "Prince in this!", "Prince in this!", "Prince in this!", "Prince in this!", 
  "Prince in this!", "Prince in this!", "Prince in this!", "",
  "", "", "", "", ">///<",
  "-", "a Tsun", ">///<", ">///<", "a TSUNDERE!",
  "", "", "", "", "", "", ">///<",">:3",">:(",">:(","Me-", "MEAN! >:(",
  ">///<", ">///<", ">///<", ">///<", "BLUSHING!",
  ">:(", ">:(", ">:(", ">:(", ">///<",
  ">;(", ">;(", ">;(", ">;<", ">;<", ">///<",
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