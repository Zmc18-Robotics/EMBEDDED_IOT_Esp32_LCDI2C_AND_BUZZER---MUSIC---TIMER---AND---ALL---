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

const int totalNada = 183;

int melodi[] = {
  // Bag 1: My heart skips a beat (11)
  RE2, MI2, DO2, RE2, MI2, DO2, RE2, RE2, DO2, RE2, MI2,
  // Bag 2: I go BOOM, You go ZOOM (10)
  DO2, DO2, SOL2, FA2, MI2,
  DO2, DO2, SOL1, RE2, DO2,
  // Bag 3: You're my playboy toy (6)
  DO2, RE2, MI2, DO2, MI2, DO2,
  // Bag 4: Lover my friend (5)
  RE2, DO2, RE2, MI2, DIAM,
  // Bag 5: I wanna be with you until the end (11)
  DO2, SOL2, SOL2, FA2, FA2, MI2,
  DO2, SOL1, RE2, DO2, DIAM,
  // -- Total: 43 --

  // Bag 6: I give my heart and my soul (10)
  FA2, FA2, FA2, FA2, MI2, FA2, SOL2, LA2, SOL2, DIAM,
  // Bag 7: To make you see it's true (8)
  DO2, LA2, SOL2, MI2, SOL2, MI2, RE2, DIAM,
  // -- Total: 61 --

  // Bag 8: I'am so confused baby can't you see (10)
  FA2, FA2, FA2, FA2, MI2, FA2, SOL2, LA2, SOL2, DIAM,
  // Bag 9: Please come rescue me (6)
  LA2, LA2, SOL2, LA2, SOL2, DIAM,
  // -- Total: 77 --

  // Bag 10: Sweet little bumblebee (6)
  MI2, MI2, DO2, FA2, FA2, DIAM,
  // Bag 11: I know what you want from me (8)
  RE2, RE2, RE2, DO2, SI1, MI2, MI2, DIAM,
  // -- Total: 91 --

  // Bag 12: Du du bi du bi (7)
  MI2, MI2, MI2, MI2, SOL2, FA2, RE2,
  // Bag 13: Du du bi du bi (8)
  SI1, SI1, SI1, SI1, FA2, MI2, DO2, DIAM,
  // -- Total: 106 --

  // Bag 14: Sweet little bumblebee (6)
  MI2, MI2, DO2, FA2, FA2, DIAM,
  // Bag 15: More than just a fantasy (8)
  RE2, RE2, RE2, DO2, SI1, MI2, MI2, DIAM,
  // -- Total: 120 --

  // Bag 16: Du du bi du bi (7)
  MI2, MI2, MI2, MI2, SOL2, FA2, RE2,
  // Bag 17: Du du bi du bi (8)
  SI1, SI1, SI1, SI1, FA2, MI2, DO2, DIAM,
  // -- Total: 135 --

  // Bag 18: Oh bridge (9)
  LA1, SOL2, FA2, RE2, SI1, FA2, MI2, DO2, DIAM,
  // Bag 19: Du du bi du bi (7)
  MI2, MI2, MI2, MI2, SOL2, FA2, RE2,
  // Bag 20: Du du bi du bi (8)
  SI1, SI1, SI1, SI1, FA2, MI2, DO2, DIAM,
  // -- Total: 159 --

  // Bag 21: Oh bridge (9)
  LA1, SOL2, FA2, RE2, SI1, FA2, MI2, DO2, DIAM,
  // Bag 22: Du du bi du bi (7)
  MI2, MI2, MI2, MI2, SOL2, FA2, RE2,
  // Bag 23: Du du bi du bi (8)
  SI1, SI1, SI1, SI1, FA2, MI2, DO2, DIAM
  // -- TOTAL: 183 --
};

int durasi[] = {
  // Bag 1 (11)
  200, 400, 200, 200, 400, 200, 200, 200, 200, 200, 400,
  // Bag 2 (10)
  200, 200, 400, 400, 400,
  200, 200, 400, 400, 400,
  // Bag 3 (6)
  200, 200, 400, 400, 400, 400,
  // Bag 4 (5)
  200, 200, 200, 600, 300,
  // Bag 5 (11)
  200, 200, 200, 200, 200, 400,
  400, 400, 200, 600, 300,

  // Bag 6 (10)
  200, 200, 200, 400, 200, 200, 400, 200, 200, 400,
  // Bag 7 (8)
  300, 300, 300, 300, 300, 300, 400, 400,

  // Bag 8 (10)
  200, 200, 200, 400, 200, 200, 400, 200, 200, 400,
  // Bag 9 (6)
  400, 400, 400, 400, 800, 800,

  // Bag 10 (6)
  350, 350, 200, 200, 350, 50,
  // Bag 11 (8)
  200, 200, 200, 200, 200, 200, 200, 100,

  // Bag 12 (7)
  100, 100, 100, 100, 300, 400, 400,
  // Bag 13 (8)
  100, 100, 100, 100, 300, 400, 400, 100,

  // Bag 14 (6)
  350, 350, 200, 200, 350, 50,
  // Bag 15 (8)
  200, 200, 200, 200, 200, 200, 200, 100,

  // Bag 16 (7)
  100, 100, 100, 100, 300, 300, 400,
  // Bag 17 (8)
  100, 100, 100, 100, 300, 300, 400, 100,

  // Bag 18 (9)
  400, 400, 400, 400, 400, 400, 400, 400, 100,
  // Bag 19 (7)
  100, 100, 100, 100, 300, 300, 400,
  // Bag 20 (8)
  100, 100, 100, 100, 300, 300, 400, 100,

  // Bag 21 (9)
  400, 400, 400, 400, 400, 400, 400, 400, 100,
  // Bag 22 (7)
  100, 100, 100, 100, 300, 300, 400,
  // Bag 23 (8)
  100, 100, 100, 100, 300, 300, 400, 100
};

const char* teks1[] = {
  // Bag 1 (11)
  "My", "My Heart", "My Heart Skips", "My Heart Skips", "My Heart Skips",
  "When", "When you", "When you walk", "When you walk", "When you walk", "When you walk",
  // Bag 2 (10)
  "I", "I go", "BOOM", "", "BOOM",
  "You", "You go", "ZOOM", "", "ZOOM",
  // Bag 3 (6)
  "You're", "You're my", "You're my", "You're my", "PLAY", "PLAY",
  // Bag 4 (5)
  "Lo-", "Lover", "Lover my", "Lover my", "Lover my",
  // Bag 5 (11)
  "I", "I wan-", "I wanna", "I wanna be", "I wanna be with", "I wanna be with",
  "I wanna be with", "I wanna be with", "The", "The", "The",

  // Bag 6 (10)
  "I", "I give", "I give my", "I give my heart", "I give my heart",
  "I give my heart", "I give my heart", "To", "To", "To",
  // Bag 7 (8)
  "To", "To make", "To make you", "To make you see",
  "To make you see", "To make you see", "To make you see", "",

  // Bag 8 (10)
  "I'am", "I'am so", "I'am so con-", "I'am so confused",
  "Ba-", "Baby", "Baby cant", "you", "you see?", "you see?",
  // Bag 9 (6)
  "Please", "Please come", "Please come", "Please come", "Please come", "Please come",

  // Bag 10 (6)
  "Sweet", "Sweet little", "Sweet little", "Sweet little", "Sweet little", "Sweet little",
  // Bag 11 (8)
  "I", "I know", "I know what", "I know what", "I know what", "From", "From me", "From me",

  // Bag 12 (7)
  "Du", "Du du", "Du du bi", "Du du bi du", "Du du bi du bi", "Du du bi du bi", "Du du bi du bi",
  // Bag 13 (8)
  "Du", "Du du", "Du du bi", "Du du bi du", "Du du bi du bi", "Du du bi du bi", "Du du bi du bi", "Du du bi du bi",

  // Bag 14 (6)
  "Sweet", "Sweet little", "Sweet little", "Sweet little", "Sweet little", "Sweet little",
  // Bag 15 (8)
  "More", "More than", "More than just", "More than just", "More than just", "More than just", "More than just", "More than just",

  // Bag 16 (7)
  "Du", "Du du", "Du du bi", "Du du bi du", "Du du bi du bi", "Du du bi du bi", "Du du bi du bi",
  // Bag 17 (8)
  "Du", "Du du", "Du du bi", "Du du bi du", "Du du bi du bi", "Du du bi du bi", "Du du bi du bi", "Du du bi du bi",

  // Bag 18 (9)
  "Oh", "Oh~", "Oh~~", "Oh~~~", "Oh~~~~", "Oh~~~~~", "Oh~~~~~~", "Oh~~~~~~~", "Oh~~~~~~~~",
  // Bag 19 (7)
  "Du", "Du du", "Du du bi", "Du du bi du", "Du du bi du bi", "Du du bi du bi", "Du du bi du bi",
  // Bag 20 (8)
  "Du", "Du du", "Du du bi", "Du du bi du", "Du du bi du bi", "Du du bi du bi", "Du du bi du bi", "Du du bi du bi",

  // Bag 21 (9)
  "Oh", "Oh~", "Oh~~", "Oh~~~", "Oh~~~~", "Oh~~~~~", "Oh~~~~~~", "Oh~~~~~~~", "Oh~~~~~~~~",
  // Bag 22 (7)
  "Du", "Du du", "Du du bi", "Du du bi du", "Du du bi du bi", "Du du bi du bi", "Du du bi du bi",
  // Bag 23 (8)
  "Du", "Du du", "Du du bi", "Du du bi du", "Du du bi du bi", "Du du bi du bi", "Du du bi du bi", "Du du bi du bi"
};

const char* teks2[] = {
  // Bag 1 (11)
  "", "", "", "a", "a beat",
  "", "", "", "In", "In the", "In the room",
  // Bag 2 (10)
  "", "", "", "BOOM", "",
  "", "", "", "ZOOM", "",
  // Bag 3 (6)
  "", "", "Play-", "PLAYBOY", "", "TOY",
  // Bag 4 (5)
  "", "", "", "Friend", "Friend",
  // Bag 5 (11)
  "", "", "", "", "", "you",
  "you un-", "you until", "", "End", "End",

  // Bag 6 (10)
  "", "", "", "", "and", "and my", "and my soul", "", "you", "you",
  // Bag 7 (8)
  "", "", "", "", "It's", "It's true", "It's true", "",

  // Bag 8 (10)
  "", "", "", "", "", "", "", "", "", "",
  // Bag 9 (6)
  "", "", "Res-", "Rescue", "Rescue me~!", "Rescue me~!",

  // Bag 10 (6)
  "", "", "Bum-", "Bumble-", "Bumblebee", "Bumblebee",
  // Bag 11 (8)
  "", "", "", "You", "You want", "", "", "",

  // Bag 12 (7)
  "", "", "", "", "", "La", "La da",
  // Bag 13 (8)
  "", "", "", "", "", "La", "La da", "La da",

  // Bag 14 (6)
  "", "", "Bum-", "Bumble-", "Bumblebee", "Bumblebee",
  // Bag 15 (8)
  "", "", "", "a", "a fan-", "a fanta-", "a fantasy", "a fantasy",

  // Bag 16 (7)
  "", "", "", "", "", "La", "La da",
  // Bag 17 (8)
  "", "", "", "", "", "La", "La da", "La da",

  // Bag 18 (9)
  "", "", "", "", "", "", "", "", "",
  // Bag 19 (7)
  "", "", "", "", "", "La", "La da",
  // Bag 20 (8)
  "", "", "", "", "", "La", "La da", "La da",

  // Bag 21 (9)
  "", "", "", "", "", "", "", "", "",
  // Bag 22 (7)
  "", "", "", "", "", "La", "La da",
  // Bag 23 (8)
  "", "", "", "", "", "La", "La da", "La da"
};


// =========================================================

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// Tampilkan LCD tanpa lcd.clear() agar tidak ada delay & lag
// Padding spasi otomatis menimpa karakter lama
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
  delay(50); // jeda antar nada
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
  // ============================================

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
