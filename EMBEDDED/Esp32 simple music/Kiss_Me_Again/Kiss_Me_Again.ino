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

// === Oktaf 0 - Minor / Semiton (rendah) ===
#define DOm     277   // C#3
#define REm     311   // D#3
#define MIm     330   // E3
#define FAm     370   // F#3
#define SOLm    415   // G#3
#define LAm     466   // A#3  ← diperbaiki
#define SIm     494   // B3   ← diperbaiki

// === Oktaf 1 - Minor / Semiton (tengah) ===
#define DOm1    554   // C#4
#define REm1    622   // D#4
#define MIm1    659   // E4
#define FAm1    740   // F#4
#define SOLm1   831   // G#4
#define LAm1    932   // A#4  ← diperbaiki
#define SIm1    988   // B4   ← diperbaiki

// === Oktaf 2 - Minor / Semiton (tinggi) ===
#define DOm2    1109  // C#5
#define REm2    1245  // D#5
#define MIm2    1319  // E5
#define FAm2    1480  // F#5
#define SOLm2   1661  // G#5
#define LAm2    1865  // A#5  ← diperbaiki
#define SIm2    1976  // B5   ← diperbaiki

// === Oktaf 3 - Minor / Semiton (sangat tinggi) ===
#define DOm3    2217  // C#6
#define REm3    2489  // D#6
#define MIm3    2637  // E6
#define FAm3    2960  // F#6
#define SOLm3   3322  // G#6
#define LAm3    3729  // A#6  ← diperbaiki
#define SIm3    3951  // B6   ← diperbaiki

// === Diam / Jeda ===
#define DIAM    0

// =========================================================
// === EDIT BAGIAN INI =====================================
// =========================================================

const int totalNada = 97;

int melodi[] = {
  FA2, FA2, MI2, FA2, LA2, FA2,
  DO2, LA1, DO2, MI2, FA2, FA2, DO2, LAm1, LA1, DIAM,

  LA2, FA2, DIAM,
  DO2, LA1, DO2, MI2, FA2, FA2, DO2, LAm1, LA1, DIAM,

  LA2, FA2, DIAM,
  DO2, LA1, DO2, MI2, FA2, FA2, DO2, LAm1, LA1, DIAM,

  LA2, FA2,
  LA2, FA2,
  SOL2, SOL2, RE2, MI2,

  LA2, FA2, DIAM,
  DO2, LA1, DO2, MI2, FA2, FA2, DO2, LAm1, LA1, DIAM,

  LA2, FA2, DIAM,
  DO2, LA1, DO2, MI2, FA2, FA2, DO2, LAm1, LA1, DIAM,

  LA2, FA2, DIAM,
  DO2, LA1, DO2, MI2, FA2, FA2, DO2, LAm1, LA1, DIAM,

  LA2, FA2,
  LA2, FA2,
  SOL2, SOL2, RE2, MI2
};

int durasi[] = {
  200, 200, 200, 200, 400, 400,
  200, 200, 200, 200, 400, 400, 200, 200, 200, 200,

  400, 400, 100,
  200, 200, 200, 200, 400, 400, 200, 200, 200, 200,

  400, 400, 100,
  200, 200, 200, 200, 400, 400, 200, 200, 200, 200,

  400, 400,
  400, 400,
  300, 300, 200, 800,

  400, 400, 100,
  200, 200, 200, 200, 400, 400, 200, 200, 200, 200,

  400, 400, 100,
  200, 200, 200, 200, 400, 400, 200, 200, 200, 200,

  400, 400, 100,
  200, 200, 200, 200, 400, 400, 200, 200, 200, 200,

  400, 400,
  400, 400,
  300, 300, 200, 800
};

const char* teks1[] = {
  // Bagian 1 - 16 elemen
  "I", "I Re-", "I Remem-", "I Remember", "I Remember", "I Remember",
  "May-", "Maybe", "Maybe was", "Maybe was the", "Maybe was the", "Maybe was the", "Of", "Of my", "Of my life", "Of my life",

  // Bagian 2 - 13 elemen
  "You", "You came", "You came",
  "To", "To my", "To my heart", "To my heart", "To my heart", "To my heart", "Open", "Open to", "Open to you", "Open to you",

  // Bagian 3 - 13 elemen
  "I", "I feel", "I feel",
  "Warm", "Warm be-", "Warm behind", "Warm behind you", "Take", "Take my", "Take my hand", "Take my hand", "Take my hand", "Take my hand",

  // Bagian 4 - 8 elemen
  "Kiss", "Kiss me",
  "Kiss", "Kiss me",
  "Kiss", "Kiss me", "Kiss me a-", "Kiss me again",

  "I", "I feel", "I feel",
  "You", "You re-", "You remem-", "You remember", "You remember", "You remember", "You remember", "To", "To love", "To love",

  "Yes", "Yes girl!", "Yes girl!",
  "Yes!", "Yes! I", "Yes! I love", "Yes! I love", "Yes! I love", "Yes! I love", "Me", "Me at", "Me at all", "",

  // Bagian 3 - 13 elemen
  "Just touch", "Just touch my", "Just touch my",
  "Lips", "Lips and", "Lips and got", "Lips and got", "Lips and got", "Lips and got", "You still", "You still", "You still", "You still",

  // Bagian 4 - 8 elemen
  "Kiss", "Kiss you",
  "Kiss", "Kiss you",
  "Kiss", "Kiss you", "Kiss you a-", "Kiss you again"
};

const char* teks2[] = {
  // Bagian 1 - 16 elemen
  "", "", "", "", "One", "One day",
  "", "", "", "", "Best", "Best day", "", "", "", "",

  // Bagian 2 - 13 elemen
  "", "", "",
  "", "", "", "My", "My eyes", "My eyes wide", "", "", "", "",

  // Bagian 3 - 13 elemen
  "", "", "",
  "", "", "", "", "", "", "", "Ins-", "Inside", "Inside",

  // Bagian 4 - 8 elemen
  "", "",
  "", "",
  "", "", "><", ">///<",

  "", "", "",
  "", "", "", "", "One", "One day", "One day I start", "", "", "",

  "", "", "",
  "", "", "", "You", "You more", "You more than", "", "", "", "",

  // Bagian 3 - 13 elemen
  "", "", "",
  "", "", "", "This", "This pic-", "This picture", "", "Remem-", "Remember", "Remember",

  // Bagian 4 - 8 elemen
  "", "",
  "", "",
  "", "", "><", ">///<"
};

// =========================================================

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

void tampilkanLCD(const char* baris1, const char* baris2) {
  char buf[17];

  lcd.setCursor(0, 0);
  snprintf(buf, sizeof(buf), "%-16s", baris1);
  lcd.print(buf);

  lcd.setCursor(0, 1);
  snprintf(buf, sizeof(buf), "%-16s", baris2);
  lcd.print(buf);
}

void mainkanNada(int frekuensi, int lama) {
  if (frekuensi == 0) {
    noTone(BUZZER_PIN);
  } else {
    tone(BUZZER_PIN, frekuensi, lama);
  }
  delay(lama);
  noTone(BUZZER_PIN);
  delay(50);
}

void setup() {
  Serial.begin(115200);

  // === Debug: validasi jumlah elemen array ===
  int j1 = sizeof(teks1) / sizeof(teks1[0]);
  int j2 = sizeof(teks2) / sizeof(teks2[0]);
  int jm = sizeof(melodi) / sizeof(melodi[0]);
  int jd = sizeof(durasi) / sizeof(durasi[0]);

  Serial.printf("teks1  = %d\n", j1);
  Serial.printf("teks2  = %d\n", j2);
  Serial.printf("melodi = %d\n", jm);
  Serial.printf("durasi = %d\n", jd);
  Serial.printf("totalNada = %d\n", totalNada);

  if (j1 != totalNada || j2 != totalNada || jm != totalNada || jd != totalNada) {
    Serial.println("ERROR: Jumlah elemen tidak sama!");
  } else {
    Serial.println("OK: Semua array sesuai.");
  }

  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("ESP32 Music     ");
  lcd.setCursor(0, 1);
  lcd.print("Siap...         ");
  delay(1500);

  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);

  for (int i = 0; i < totalNada; i++) {
    tampilkanLCD(teks1[i], teks2[i]);
    mainkanNada(melodi[i], durasi[i]);
  }

  lcd.setCursor(0, 0);
  lcd.print("Selesai!        ");
  lcd.setCursor(0, 1);
  lcd.print("EN untuk ulangi ");
}

void loop() {
  // Tekan EN/BOOT untuk mengulang
}