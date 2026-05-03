/*
 * ESP32 Music Player - Happy Birthday To You
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

// === Nada ===
#define SOL1    784
#define LA1     880
#define SI1     988
#define DO2     1047
#define RE2     1175
#define MI2     1319
#define FA2     1397
#define SOL2    1568

// === Tempo ===
#define T1   375
#define T15  562
#define T2   750
#define T3   1125
#define T4   1500

// =========================================================
// Custom Character Index
// =========================================================
#define CHAR_NOTE1   0   // not penuh  ♩
#define CHAR_NOTE2   1   // not ganda  ♫
#define CHAR_BAR_LO  2   // bar rendah  [_]
#define CHAR_BAR_MID 3   // bar tengah  [=]
#define CHAR_BAR_HI  4   // bar tinggi  [■]
#define CHAR_HEART   5   // hati ♥
#define CHAR_STAR    6   // bintang ★
#define CHAR_CAKE    7   // kue ulang tahun

// Bitmap custom character (5x8 pixel)
byte charNote1[8]  = { 0x04, 0x06, 0x05, 0x04, 0x04, 0x0C, 0x0C, 0x00 };
byte charNote2[8]  = { 0x0F, 0x09, 0x09, 0x01, 0x01, 0x07, 0x07, 0x00 };
byte charBarLo[8]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00 };
byte charBarMid[8] = { 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x00 };
byte charBarHi[8]  = { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00 };
byte charHeart[8]  = { 0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00 };
byte charStar[8]   = { 0x00, 0x04, 0x15, 0x0E, 0x1F, 0x0E, 0x15, 0x04 };
byte charCake[8]   = { 0x04, 0x04, 0x0E, 0x1F, 0x1B, 0x1F, 0x1F, 0x00 };

// =========================================================
// Data Lagu
// =========================================================
const int totalNada = 25;

int melodi[] = {
  SOL1, SOL1, LA1,  SOL1, DO2,  SI1,
  SOL1, SOL1, LA1,  SOL1, RE2,  DO2,
  SOL1, SOL1, SOL2, MI2,  DO2,  SI1,  LA1,
  FA2,  FA2,  MI2,  DO2,  RE2,  DO2
};

int durasi[] = {
  T1, T1, T2, T2, T2, T4,
  T1, T1, T2, T2, T2, T4,
  T1, T1, T2, T2, T15, T1, T4,
  T1, T1, T2, T2, T2, T4
};

const char* teks1[] = {
  "Hap-",  "py",   "Birth-", "day",  "to",   "you!",
  "Hap-",  "py",   "Birth-", "day",  "to",   "you!",
  "Hap-",  "py",   "Birth-", "day,", "dear", "...",  "Friend!",
  "Hap-",  "py",   "Birth-", "day",  "to",   "you!"
};

// =========================================================
// Emoji baris 2 per nada:
// Format: { charIndex1, charIndex2, charIndex3, ... }
// Kita pakai struct sederhana: 4 slot karakter untuk baris 2
// =========================================================

// Fungsi penentu ikon berdasarkan frekuensi nada
// Oktaf rendah (SOL1, LA1, SI1)  -> BAR_LO
// Oktaf tengah (DO2, RE2, MI2)   -> BAR_MID
// Oktaf tinggi (FA2, SOL2+)      -> BAR_HI

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// =========================================================
// Tentukan level oktaf dari frekuensi
// 0 = rendah, 1 = tengah, 2 = tinggi
// =========================================================
int getOktafLevel(int freq) {
  if (freq <= 988)  return 0; // SOL1, LA1, SI1
  if (freq <= 1319) return 1; // DO2, RE2, MI2
  return 2;                   // FA2, SOL2, dst
}

// =========================================================
// Tampilkan baris 2 dengan custom char sesuai nada & urutan
// =========================================================
void tampilkanBaris2(int freq, int nadaKe) {
  lcd.setCursor(0, 1);

  int level = getOktafLevel(freq);
  byte barChar = (level == 0) ? CHAR_BAR_LO :
                 (level == 1) ? CHAR_BAR_MID : CHAR_BAR_HI;

  // Not bergantian: genap = NOTE1, ganjil = NOTE2
  byte notChar = (nadaKe % 2 == 0) ? CHAR_NOTE1 : CHAR_NOTE2;

  // Layout baris 2 (16 kolom):
  // [not] [spasi] [bar][bar][bar] [spasi] [heart/star/cake] [spasi] [not]
  // Contoh: ♩ ▄▄▄   ♥   ♩

  // Ikon kanan bergantian per baris lagu
  byte ikonKanan;
  if      (nadaKe < 6)  ikonKanan = CHAR_HEART;  // baris 1
  else if (nadaKe < 12) ikonKanan = CHAR_STAR;   // baris 2
  else if (nadaKe < 19) ikonKanan = CHAR_CAKE;   // baris 3
  else                  ikonKanan = CHAR_HEART;  // baris 4

  // Tulis ke LCD
  lcd.write(notChar);
  lcd.print(" ");
  lcd.write(barChar);
  lcd.write(barChar);
  lcd.write(barChar);
  lcd.print(" ");
  lcd.write(ikonKanan);
  lcd.print(" ");
  lcd.write(notChar);
  lcd.print("            "); // bersihkan sisa kolom
}

// =========================================================
void tampilkanLCD(const char* baris1, int freq, int nadaKe) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(baris1);
  tampilkanBaris2(freq, nadaKe);
}

void mainkanNada(int frekuensi, int lama) {
  if (frekuensi == 0) {
    noTone(BUZZER_PIN);
    delay(lama);
  } else {
    tone(BUZZER_PIN, frekuensi, lama);
    delay(lama);
    noTone(BUZZER_PIN);
  }
  delay(60);
}

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);

  lcd.init();
  lcd.backlight();

  // Daftarkan semua custom character
  lcd.createChar(CHAR_NOTE1,  charNote1);
  lcd.createChar(CHAR_NOTE2,  charNote2);
  lcd.createChar(CHAR_BAR_LO, charBarLo);
  lcd.createChar(CHAR_BAR_MID,charBarMid);
  lcd.createChar(CHAR_BAR_HI, charBarHi);
  lcd.createChar(CHAR_HEART,  charHeart);
  lcd.createChar(CHAR_STAR,   charStar);
  lcd.createChar(CHAR_CAKE,   charCake);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Happy Birthday!");
  lcd.setCursor(0, 1);
  lcd.write(CHAR_CAKE);
  lcd.print(" Siap...");
  delay(2000);

  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);

  // Mainkan lagu
  for (int i = 0; i < totalNada; i++) {
    tampilkanLCD(teks1[i], melodi[i], i);
    mainkanNada(melodi[i], durasi[i]);
  }

  // Selesai
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Happy Birthday!");
  lcd.setCursor(0, 1);
  lcd.write(CHAR_CAKE);
  lcd.print(" ");
  lcd.write(CHAR_HEART);
  lcd.print(" Selesai! ");
  lcd.write(CHAR_STAR);
}

void loop() {
  // Kosong
}