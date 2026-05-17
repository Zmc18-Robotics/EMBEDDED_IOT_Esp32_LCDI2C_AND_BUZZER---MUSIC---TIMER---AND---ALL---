/*
 * =====================================================================
 * ESP32 IoT WiFi Music Player + Countdown Timer (PREMIUM VERSION)
 * Kontrol penuh via Web Browser (WiFi)
 * =====================================================================
 * Hardware Pinout:
 *   - I2C SDA       -> GPIO 21
 *   - I2C SCL       -> GPIO 22
 *   - Buzzer        -> GPIO 5
 *   - RGB LED RED   -> GPIO 4
 *   - RGB LED GREEN -> GPIO 2
 *   - RGB LED BLUE  -> GPIO 13
 *   - Relay Module  -> GPIO 27
 *   - LCD I2C Addr  -> 0x27 (16x2)
 *
 * Library Required:
 *   - LiquidCrystal_I2C by Frank de Brabander
 *   - WiFi (Built-in)
 *   - WebServer (Built-in)
 *
 * Fitur Utama:
 *   - 9 lagu: Fur Elise, Happy Birthday, Its Not Like I Like You,
 *             Mayor Minor Scale, Nokia Ringtone, Super Mario Theme,
 *             Sweet Little Bumblebee, Kiss Me Again, Tetris Theme
 *   - Countdown Timer Otomatis (1-3600 detik)
 *   - Music Sync RGB LED (Warna berubah mengikuti frekuensi nada)
 *   - Relay Control (Otomatis aktif setelah Timer habis)
 *   - LCD Dashboard dengan Animasi Lirik & Karakter Spesial
 *   - Web UI Premium yang Responsif & Interactive
 * =====================================================================
 */
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
#include <LiquidCrystal_I2C.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Wire.h>

// =====================================================================
// === KONFIGURASI WiFi — UBAH INI SESUAI JARINGAN ANDA ================
// =====================================================================
const char *WIFI_SSID = "YOUR_WIFI_NAME";
const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
// =====================================================================

#define BUZZER_PIN 5
#define SDA_PIN 21
#define SCL_PIN 22
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

#define PIN_LED_R 4
#define PIN_LED_G 2
#define PIN_LED_B 13
#define PIN_RELAY 27

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
WebServer server(80);

// =====================================================================
// === STATE MESIN =====================================================
// =====================================================================
volatile bool stopFlag = false;
volatile bool isPlaying = false;
volatile bool isTimer = false;
volatile int currentSong = -1;
volatile int timerSeconds = 15;

// LED & Relay State
bool ledOn = true;
bool ledSync = true;
int ledR = 255, ledG = 0, ledB = 0;
bool relayState = false;

// =====================================================================
// === FREKUENSI NADA ==================================================
// =====================================================================
// Oktaf 0 Mayor
#define DO 262
#define RE 294
#define MI 330
#define FA 349
#define SOL 392
#define LA 440
#define SI 494
// Oktaf 1 Mayor
#define DO1 523
#define RE1 587
#define MI1 659
#define FA1 698
#define SOL1 784
#define LA1 880
#define SI1 988
// Oktaf 2 Mayor
#define DO2 1047
#define RE2 1175
#define MI2 1319
#define FA2 1397
#define SOL2 1568
#define LA2 1760
#define SI2 1976
// Oktaf 3 Mayor
#define DO3 2093
#define RE3 2349
#define MI3 2637
// Oktaf 0 Minor
#define DOm 277
#define REm 311
#define MIm 311
#define FAm 370
#define SOLm 415
#define LAm 415
#define SIm 466
// === Oktaf 0 - Minor / Semiton (rendah) ===
#define DOm 277  // C#3
#define REm 311  // D#3
#define MIm 330  // E3
#define FAm 370  // F#3
#define SOLm 415 // G#3
#define LAm 466  // A#3
#define SIm 494  // B3
// === Oktaf 1 - Minor / Semiton (tengah) ===
#define DOm1 554  // C#4
#define REm1 622  // D#4
#define MIm1 659  // E4
#define FAm1 740  // F#4
#define SOLm1 831 // G#4
#define LAm1 932  // A#4
#define SIm1 988  // B4
// === Oktaf 2 - Minor / Semiton (tinggi) ===
#define DOm2 1109  // C#5
#define REm2 1245  // D#5
#define MIm2 1319  // E5
#define FAm2 1480  // F#5
#define SOLm2 1661 // G#5
#define LAm2 1865  // A#5
#define SIm2 1976  // B5
// === Oktaf 3 - Minor / Semiton (sangat tinggi) ===
#define DOm3 2217  // C#6
#define REm3 2489  // D#6
#define MIm3 2637  // E6
#define FAm3 2960  // F#6
#define SOLm3 3322 // G#6
#define LAm3 3729  // A#6
#define SIm3 3951  // B6
// Nada Tetris
#define E4 330
#define F4 349
#define G4 392
#define A4 440
#define B4 494
#define C5 523
#define D5 587
#define E5 659
#define F5 698
#define G5 784
#define A5 880
#define C6 1047
#define AS4 466
#define DS5 622
#define DS4 311
#define CS4 277
#define GS3 208
#define C3 131
#define E3 165
#define A3 220
#define B3 247
// Oktaf untuk Happy Birthday
#define SOL_HB 784
#define LA_HB 880
#define SI_HB 988
#define DO2_HB 1047
#define RE2_HB 1175
#define MI2_HB 1319
#define FA2_HB 1397
#define SOL2_HB 1568
// Diam
#define DIAM 0

// Nada khusus Fur Elise (prefix FE_ agar tidak konflik)
#define FE_E4 330
#define FE_DS4 311
#define FE_D4 294
#define FE_C4 262
#define FE_B3 247
#define FE_A3 220
#define FE_GS3 208
#define FE_E3 165
#define FE_C3 131

// =====================================================================
// === TEMPO HELPER ====================================================
// =====================================================================
#define BPM_FUR 120
#define BEAT_FUR (60000 / BPM_FUR)
#define Q_FUR BEAT_FUR
#define H_FUR (BEAT_FUR * 2)
#define E_FUR (BEAT_FUR / 2)
#define S_FUR (BEAT_FUR / 4)
#define DQ_FUR (BEAT_FUR * 3 / 2)
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
#define BPM_TET 144
#define BEAT_TET (60000 / BPM_TET)
#define Q_TET BEAT_TET
#define H_TET (BEAT_TET * 2)
#define E_TET (BEAT_TET / 2)
#define DQ_TET (BEAT_TET * 3 / 2)

// =====================================================================
// === LAGU 1: FUR ELISE — BEETHOVEN ===================================
// =====================================================================
const int furEliseTotal = 44;
const int furEliseMelodi[] = {
    FE_E4, FE_DS4, FE_E4,  FE_DS4, FE_E4,  FE_B3, FE_D4,  FE_C4, FE_A3, DIAM,
    FE_C3, FE_E3,  FE_A3,  FE_B3,  DIAM,   FE_E3, FE_GS3, FE_B3, FE_C4, DIAM,
    FE_E3, FE_E4,  FE_DS4, FE_E4,  FE_DS4, FE_E4, FE_B3,  FE_D4, FE_C4, FE_A3,
    DIAM,  FE_C3,  FE_E3,  FE_A3,  FE_B3,  DIAM,  FE_E3,  FE_C4, FE_B3, FE_A3,
    DIAM,  DIAM,   DIAM,   FE_E4,  FE_DS4, FE_E4, FE_DS4};
const int furEliseDurasi[] = {
    S_FUR, S_FUR, S_FUR, S_FUR, S_FUR, S_FUR, S_FUR, S_FUR, Q_FUR, E_FUR,
    S_FUR, S_FUR, S_FUR, Q_FUR, E_FUR, S_FUR, S_FUR, S_FUR, Q_FUR, E_FUR,
    S_FUR, S_FUR, S_FUR, S_FUR, S_FUR, S_FUR, S_FUR, S_FUR, S_FUR, Q_FUR,
    E_FUR, S_FUR, S_FUR, S_FUR, Q_FUR, E_FUR, S_FUR, S_FUR, S_FUR, Q_FUR,
    E_FUR, E_FUR, E_FUR, S_FUR, S_FUR, S_FUR, S_FUR};
const char *furEliseTeks1[] = {
    "Fur Elise~", "da~",     "da~",       "da~", "Beethoven", "da~", "da~",
    "da~",        "1810~~",  "...",       "da~", "da~",       "da~", "Piano~~",
    "...",        "da~",     "da~",       "da~", "Classic~",  "...", "da~",
    "Fur~",       "da~",     "Elise~~",   "da~", "da~",       "da~", "da~",
    "da~",        "Ludwig~", "...",       "da~", "da~",       "da~", "van~~",
    "...",        "da~",     "Beethoven", "da~", "1810~~",    "...", "...",
    "...",        "Fur~",    "Elise~",    "da~", "da~"};
const char *furEliseTeks2[] = {
    "~da da da~", "~",           "~",       "~",       "~da da~", "~",
    "~",          "~",           "~1810~",  "~",       "~",       "~",
    "~",          "~piano~",     "~",       "~",       "~",       "~",
    "~classic~",  "~",           "~",       "~fur~",   "~",       "~elise~",
    "~",          "~",           "~",       "~",       "~",       "~ludwig~",
    "~",          "~",           "~",       "~",       "~van~",   "~",
    "~",          "~beethoven~", "~",       "(*^_^*)", "~",       "~",
    "~",          "~fur~",       "~elise~", "~da~",    "~da~"};

// =====================================================================
// === LAGU 2: HAPPY BIRTHDAY ==========================================
// =====================================================================
#define T1_HB 375
#define T15_HB 562
#define T2_HB 750
#define T3_HB 1125
#define T4_HB 1500

const int happyBirthTotal = 25;
const int happyBirthMelodi[] = {
    SOL_HB, SOL_HB, LA_HB,  SOL_HB, DO2_HB, SI_HB,   SOL_HB, SOL_HB, LA_HB,
    SOL_HB, RE2_HB, DO2_HB, SOL_HB, SOL_HB, SOL2_HB, MI2_HB, DO2_HB, SI_HB,
    LA_HB,  FA2_HB, FA2_HB, MI2_HB, DO2_HB, RE2_HB,  DO2_HB};
const int happyBirthDurasi[] = {
    T1_HB, T1_HB, T2_HB, T2_HB, T2_HB, T4_HB, T1_HB, T1_HB,  T2_HB,
    T2_HB, T2_HB, T4_HB, T1_HB, T1_HB, T2_HB, T2_HB, T15_HB, T1_HB,
    T4_HB, T1_HB, T1_HB, T2_HB, T2_HB, T2_HB, T4_HB};
const char *happyBirthTeks1[] = {
    "Hap-",   "py",     "Birth-", "day", "to",      "you!", "Hap-",
    "py",     "Birth-", "day",    "to",  "you!",    "Hap-", "py",
    "Birth-", "day,",   "dear",   "...", "Friend!", "Hap-", "py",
    "Birth-", "day",    "to",     "you!"};
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
// =====================================================================
// === LAGU 3: ITS NOT LIKE I LIKE YOU =================================
// =====================================================================
const int itsNotTotal = 49;
const int itsNotMelodi[] = {
    SOL1,  SOL1,  SOL1,  MI2,   RE2,  DO2,   DO2, LA1, DO2, DIAM,
    LA2,   SOL2,  FA2,   SOL2,  LA2,  MI2,   RE2, DO2, RE2, DO2,
    SOLm2, SOLm2, SOLm2, SOLm2, SOL2, SOLm2, MI3, RE3, DO3, DO3,
    RE3,   LA2,   LA2,   SOL2,  FA2,  SOL2,  LA2, MI2, RE2, DO2,
    RE2,   DO2,   SOL2,  SOL2,  MI3,  DO3,   RE2, DO2, DIAM};
const int itsNotDurasi[] = {250, 250, 250, 300, 200,  250, 350, 150, 400, 1500,
                            400, 200, 200, 200, 500,  400, 200, 200, 200, 500,
                            150, 150, 150, 150, 150,  300, 600, 200, 200, 200,
                            200, 400, 150, 150, 150,  150, 600, 250, 150, 150,
                            150, 600, 800, 400, 1000, 200, 200, 600, 1000};
const char *itsNotTeks1[] = {"You are not My",
                             "You are not My",
                             "You are not My",
                             "You are not My",
                             "You are not My",
                             "You are not My",
                             "You are not My",
                             "You are not My",
                             "You are not My",
                             "",
                             "I",
                             "I am",
                             "I am not",
                             "I am not a",
                             "I am not a DERE!",
                             "a Tsun",
                             ">///<",
                             "a Tsun",
                             "a Tsun-Dere",
                             ">///<",
                             "I",
                             "I Have",
                             "I Have a",
                             "I Have a",
                             "I Have a jel-",
                             "I Have a jello",
                             "HEART!",
                             "I'm",
                             "I'm not",
                             "I'm not that",
                             "I'am not that",
                             "I'am not that",
                             "I'm",
                             "I'm",
                             "I'am not",
                             "I'am not blush-",
                             ">///< >:(",
                             "I",
                             "I was",
                             "I was just",
                             "I was just rush-",
                             "RUSHING!!!",
                             "andddd",
                             "for-",
                             "forgotttt",
                             "the",
                             "Sun-",
                             "SUNSCREEN!",
                             ""};
const char *itsNotTeks2[] = {"Prince in this!",
                             "Prince in this!",
                             "Prince in this!",
                             "Prince in this!",
                             "Prince in this!",
                             "Prince in this!",
                             "Prince in this!",
                             "Prince in this!",
                             "Prince in this!",
                             "",
                             "",
                             "",
                             "",
                             "",
                             ">///<",
                             "-",
                             "a Tsun",
                             ">///<",
                             ">///<",
                             "a TSUNDERE!",
                             "",
                             "",
                             "",
                             "",
                             "",
                             "",
                             ">///<",
                             ">:3",
                             ">:(",
                             ">(",
                             "Me-",
                             "MEAN! >:(",
                             ">///<",
                             ">///<",
                             ">///<",
                             ">///<",
                             "BLUSHING!",
                             ">:(",
                             ">:(",
                             ">:(",
                             ">:(",
                             ">///<",
                             ">;(",
                             ">;(",
                             ">;(",
                             ">;<",
                             ">;<",
                             ">///<",
                             ""};

// =====================================================================
// === LAGU 4: MAYOR MINOR SCALE =======================================
// =====================================================================
const int mayorMinorTotal = 6;
const int mayorMinorMelodi[] = {DO2, RE2, MI2, MIm2, REm2, DOm2};
const int mayorMinorDurasi[] = {400, 400, 400, 400, 400, 600};
const char *mayorMinorTeks1[] = {"Do Mayor", "Re Mayor", "Mi Mayor",
                                 "Mi Minor", "Re Minor", "Do Minor"};
const char *mayorMinorTeks2[] = {"Oktaf 2", "Oktaf 2", "Oktaf 2",
                                 "Oktaf 2", "Oktaf 2", "Oktaf 2"};
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
// =====================================================================
// === LAGU 5: NOKIA RINGTONE ==========================================
// =====================================================================
const int nokiaTotal = 31;
const int nokiaMelodi[] = {MI2, RE2,  FA1, LA1,  DO2, SI1,  MI1, SOL1,
                           LA1, SOL1, DO2, MI2,  LA2, DIAM, MI2, RE2,
                           FA1, LA1,  DO2, SI1,  MI1, SOL1, LA1, SOL1,
                           DO2, MI2,  LA2, SOL2, MI2, DO2,  DIAM};
const int nokiaDurasi[] = {200, 200, 150, 400, 200, 200, 150, 400,
                           200, 200, 150, 400, 500, 600, 200, 200,
                           150, 400, 200, 200, 150, 400, 200, 200,
                           150, 400, 300, 250, 250, 400, 1000};
const char *nokiaTeks1[] = {"Nokia...",
                            "Nokia..",
                            "Nokia.",
                            "Nokia!",
                            "Gran Vals",
                            "Gran Vals~",
                            "~",
                            "Nokia!",
                            "Francisco",
                            "Tarrega",
                            "1902",
                            "Nokia!",
                            "** NOKIA **",
                            "",
                            "Nokia...",
                            "Nokia..",
                            "Nokia.",
                            "Nokia!",
                            "Calling...",
                            "Calling..",
                            "Calling.",
                            "INCOMING!",
                            "Ring ring~",
                            "Ring ring!",
                            "~Nokia~",
                            "NOKIA!!!",
                            "Gotta pick",
                            "up that",
                            "classic",
                            "ringtone~",
                            ""};
const char *nokiaTeks2[] = {"da...",
                            "da da..",
                            "da da da..",
                            "DUM!",
                            "da...",
                            "da da..",
                            "da da da..",
                            "DUM!",
                            "da...",
                            "da da..",
                            "da da da..",
                            "DUM!",
                            "~3310 vibes~",
                            "",
                            "da...",
                            "da da..",
                            "da da da..",
                            "DUM!",
                            "brr...",
                            "brrring..",
                            "RRIING!!",
                            ">phonepickup<",
                            "~",
                            "~*~",
                            "~*~*~",
                            "(*.*)/",
                            "from 2003",
                            "flip open",
                            "hello?",
                            "(^_^)/",
                            ""};

// =====================================================================
// === LAGU 6: SUPER MARIO THEME =======================================
// =====================================================================
const int marioTotal = 39;
const int marioMelodi[] = {
    E5,   E5,   DIAM, E5, DIAM, C5,   E5,   DIAM, G5,   DIAM, DIAM, DIAM, G4,
    DIAM, DIAM, DIAM, C5, DIAM, DIAM, G4,   DIAM, DIAM, E4,   DIAM, DIAM, A4,
    DIAM, B4,   DIAM, A4, G4,   E5,   DIAM, G5,   A5,   DIAM, F5,   G5,   DIAM};
const int marioDurasi[] = {150, 150, 150, 150, 150, 150, 150, 150, 300, 150,
                           150, 300, 300, 150, 150, 300, 300, 150, 300, 300,
                           150, 300, 150, 150, 300, 150, 150, 150, 150, 200,
                           200, 200, 200, 200, 200, 150, 150, 200, 300};
const char *marioTeks1[] = {
    "Super Mario!", "Super Mario!", "...",       "Super Mario!", "...",
    "Go Mario!",    "Go Mario!",    "...",       "World 1-1",    "...",
    "...",          "World 1-1",    "Goomba!",   "...",          "...",
    "Jump!",        "1UP!",         "...",       "Coins:",       "Coins:x3",
    "...",          "Coins:x5",     "Run!",      "...",          "...",
    "Princess!",    "...",          "Save her!", "...",          "Wahoo!",
    "YA-HOO!",      "...",          "Mario!!",   "MARIO!!",      "...",
    "YEAH!!",       "...",          "Its-a me",  "Mario!!!",     "WINNER!"};
const char *marioTeks2[] = {
    "da da~",     "da DUM!",    "~",           "da da da~",   "~",
    "DUM DUM!",   "DUM!",       "~",           "~coin~",      "~",
    "~",          "da da DUM!", "stomp!",      "~",           "~",
    "Wheee~",     "~",          "~",           "*bing!*",     "*bing bing!*",
    "~",          "*bing!*",    "da da da~",   "~",           "~",
    "da da DUM!", "~",          "da da~",      "~",           "WA-HOO!",
    "(*^o^*)",    "~",          "da DUM DUM~", "da da DUM!!", "~",
    "(*^_^*)/",   "~",          "~",           "(^o^)/",      "GAME CLEAR!"};

// =====================================================================
// === LAGU 7: SWEET LITTLE BUMBLEBEE ==================================
// =====================================================================
const int bumblebeeTotal = 183;
const int bumblebeeMelodi[] = {
    RE2,  MI2,  DO2,  RE2,  MI2,  DO2,  RE2,  RE2,  DO2,  RE2, MI2,  DO2,  DO2,
    SOL2, FA2,  MI2,  DO2,  DO2,  SOL1, RE2,  DO2,  DO2,  RE2, MI2,  DO2,  MI2,
    DO2,  RE2,  DO2,  RE2,  MI2,  DIAM, DO2,  SOL2, SOL2, FA2, FA2,  MI2,  DO2,
    SOL1, RE2,  DO2,  DIAM, FA2,  FA2,  FA2,  FA2,  MI2,  FA2, SOL2, LA2,  SOL2,
    DIAM, DO2,  LA2,  SOL2, MI2,  SOL2, MI2,  RE2,  DIAM, FA2, FA2,  FA2,  FA2,
    MI2,  FA2,  SOL2, LA2,  SOL2, DIAM, LA2,  LA2,  SOL2, LA2, SOL2, DIAM, MI2,
    MI2,  DO2,  FA2,  FA2,  DIAM, RE2,  RE2,  RE2,  DO2,  SI1, MI2,  MI2,  DIAM,
    MI2,  MI2,  MI2,  MI2,  SOL2, FA2,  RE2,  SI1,  SI1,  SI1, SI1,  FA2,  MI2,
    DO2,  DIAM, MI2,  MI2,  DO2,  FA2,  FA2,  DIAM, RE2,  RE2, RE2,  DO2,  SI1,
    MI2,  MI2,  DIAM, MI2,  MI2,  MI2,  MI2,  SOL2, FA2,  RE2, SI1,  SI1,  SI1,
    SI1,  FA2,  MI2,  DO2,  DIAM, LA1,  SOL2, FA2,  RE2,  SI1, FA2,  MI2,  DO2,
    DIAM, MI2,  MI2,  MI2,  MI2,  SOL2, FA2,  RE2,  SI1,  SI1, SI1,  SI1,  FA2,
    MI2,  DO2,  DIAM, LA1,  SOL2, FA2,  RE2,  SI1,  FA2,  MI2, DO2,  DIAM, MI2,
    MI2,  MI2,  MI2,  SOL2, FA2,  RE2,  SI1,  SI1,  SI1,  SI1, FA2,  MI2,  DO2,
    DIAM};
const int bumblebeeDurasi[] = {
    200, 400, 200, 200, 400, 200, 200, 200, 200, 200, 400, 200, 200, 400, 400,
    400, 200, 200, 400, 400, 400, 200, 200, 400, 400, 400, 400, 200, 200, 200,
    600, 300, 200, 200, 200, 200, 200, 400, 400, 400, 200, 600, 300, 200, 200,
    200, 400, 200, 200, 400, 200, 200, 400, 300, 300, 300, 300, 300, 300, 400,
    400, 200, 200, 200, 400, 200, 200, 400, 200, 200, 400, 400, 400, 400, 400,
    800, 800, 350, 350, 200, 200, 350, 50,  200, 200, 200, 200, 200, 200, 200,
    100, 100, 100, 100, 100, 300, 400, 400, 100, 100, 100, 100, 300, 400, 400,
    100, 350, 350, 200, 200, 350, 50,  200, 200, 200, 200, 200, 200, 200, 100,
    100, 100, 100, 100, 300, 300, 400, 100, 100, 100, 100, 300, 300, 400, 100,
    400, 400, 400, 400, 400, 400, 400, 400, 100, 100, 100, 100, 100, 300, 300,
    400, 100, 100, 100, 100, 300, 300, 400, 100, 400, 400, 400, 400, 400, 400,
    400, 400, 100, 100, 100, 100, 100, 300, 300, 400, 100, 100, 100, 100, 300,
    300, 400, 100};
const char *bumblebeeTeks1[] = {"My",
                                "My Heart",
                                "My Heart Skips",
                                "My Heart Skips",
                                "My Heart Skips",
                                "When",
                                "When you",
                                "When you walk",
                                "When you walk",
                                "When you walk",
                                "When you walk",
                                "I",
                                "I go",
                                "BOOM",
                                "",
                                "BOOM",
                                "You",
                                "You go",
                                "ZOOM",
                                "",
                                "ZOOM",
                                "You're",
                                "You're my",
                                "You're my",
                                "You're my",
                                "PLAY",
                                "PLAY",
                                "Lo-",
                                "Lover",
                                "Lover my",
                                "Lover my",
                                "Lover my",
                                "I",
                                "I wan-",
                                "I wanna",
                                "I wanna be",
                                "I wanna be with",
                                "I wanna be with",
                                "I wanna be with",
                                "I wanna be with",
                                "The",
                                "The",
                                "The",
                                "I",
                                "I give",
                                "I give my",
                                "I give my heart",
                                "I give my heart",
                                "I give my heart",
                                "I give my heart",
                                "To",
                                "To",
                                "To",
                                "To",
                                "To make",
                                "To make you",
                                "To make you see",
                                "To make you see",
                                "To make you see",
                                "To make you see",
                                "",
                                "I'am",
                                "I'am so",
                                "I'am so con-",
                                "I'am so confused",
                                "Ba-",
                                "Baby",
                                "Baby cant",
                                "you",
                                "you see?",
                                "you see?",
                                "Please",
                                "Please come",
                                "Please come",
                                "Please come",
                                "Please come",
                                "Please come",
                                "Sweet",
                                "Sweet little",
                                "Sweet little",
                                "Sweet little",
                                "Sweet little",
                                "Sweet little",
                                "I",
                                "I know",
                                "I know what",
                                "I know what",
                                "I know what",
                                "From",
                                "From me",
                                "From me",
                                "Du",
                                "Du du",
                                "Du du bi",
                                "Du du bi du",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Du",
                                "Du du",
                                "Du du bi",
                                "Du du bi du",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Sweet",
                                "Sweet little",
                                "Sweet little",
                                "Sweet little",
                                "Sweet little",
                                "Sweet little",
                                "More",
                                "More than",
                                "More than just",
                                "More than just",
                                "More than just",
                                "More than just",
                                "More than just",
                                "More than just",
                                "Du",
                                "Du du",
                                "Du du bi",
                                "Du du bi du",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Du",
                                "Du du",
                                "Du du bi",
                                "Du du bi du",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Oh",
                                "Oh~",
                                "Oh~~",
                                "Oh~~~",
                                "Oh~~~~",
                                "Oh~~~~~",
                                "Oh~~~~~~",
                                "Oh~~~~~~~",
                                "Oh~~~~~~~~",
                                "Du",
                                "Du du",
                                "Du du bi",
                                "Du du bi du",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Du",
                                "Du du",
                                "Du du bi",
                                "Du du bi du",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Oh",
                                "Oh~",
                                "Oh~~",
                                "Oh~~~",
                                "Oh~~~~",
                                "Oh~~~~~",
                                "Oh~~~~~~",
                                "Oh~~~~~~~",
                                "Oh~~~~~~~~",
                                "Du",
                                "Du du",
                                "Du du bi",
                                "Du du bi du",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Du",
                                "Du du",
                                "Du du bi",
                                "Du du bi du",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Du du bi du bi",
                                "Du du bi du bi"};
const char *bumblebeeTeks2[] = {"",
                                "",
                                "",
                                "a",
                                "a beat",
                                "",
                                "",
                                "",
                                "In",
                                "In the",
                                "In the room",
                                "",
                                "",
                                "",
                                "BOOM",
                                "",
                                "",
                                "",
                                "",
                                "ZOOM",
                                "",
                                "",
                                "",
                                "Play-",
                                "PLAYBOY",
                                "",
                                "TOY",
                                "",
                                "",
                                "",
                                "Friend",
                                "Friend",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "you",
                                "you un-",
                                "you until",
                                "",
                                "End",
                                "End",
                                "",
                                "",
                                "",
                                "",
                                "and",
                                "and my",
                                "and my soul",
                                "",
                                "you",
                                "you",
                                "",
                                "",
                                "",
                                "",
                                "It's",
                                "It's true",
                                "It's true",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "Res-",
                                "Rescue",
                                "Rescue me~!",
                                "Rescue me~!",
                                "",
                                "",
                                "Bum-",
                                "Bumble-",
                                "Bumblebee",
                                "Bumblebee",
                                "",
                                "",
                                "",
                                "You",
                                "You want",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "La",
                                "La da",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "La",
                                "La da",
                                "La da",
                                "",
                                "",
                                "Bum-",
                                "Bumble-",
                                "Bumblebee",
                                "Bumblebee",
                                "",
                                "",
                                "",
                                "a",
                                "a fan-",
                                "a fanta-",
                                "a fantasy",
                                "a fantasy",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "La",
                                "La da",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "La",
                                "La da",
                                "La da",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "La",
                                "La da",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "La",
                                "La da",
                                "La da",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "La",
                                "La da",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "La",
                                "La da",
                                "La da"};
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
// =====================================================================
// === LAGU 8: TETRIS THEME ============================================
// =====================================================================
const int tetrisTotal = 40;
const int tetrisMelodi[] = {E5, B4, C5, D5, C5, B4, A4, A4,   C5, E5,
                            D5, C5, B4, C5, D5, E5, C5, A4,   A4, D5,
                            F5, A5, G5, F5, E5, C5, E5, D5,   C5, B4,
                            B4, C5, D5, E5, C5, A4, A4, DIAM, E5, E5};
const int tetrisDurasi[] = {
    Q_TET, E_TET, E_TET,  Q_TET, E_TET,  E_TET, Q_TET, E_TET, E_TET, Q_TET,
    E_TET, E_TET, DQ_TET, E_TET, Q_TET,  Q_TET, Q_TET, Q_TET, H_TET, Q_TET,
    E_TET, Q_TET, E_TET,  E_TET, DQ_TET, E_TET, Q_TET, E_TET, E_TET, Q_TET,
    E_TET, E_TET, Q_TET,  Q_TET, Q_TET,  Q_TET, H_TET, E_TET, Q_TET, H_TET};
const char *tetrisTeks1[] = {
    "Tetris!!", "da~",       "da~",      "TETRIS!",  "da~",    "da~",
    "Block!!",  "da~",       "da~",      "Drop it!", "da~",    "da~",
    "Line~~",   "da~",       "Clear!!",  "YEAH!!",   "da da~", "Score!!",
    "TETRIS!!", "Level UP!", "da~",      "Faster!!", "da~",    "da~",
    "T-Spin!!", "da~",       "Combo!!",  "da~",      "da~",    "4 Lines!",
    "da~",      "da~",       "TETRIS!!", "CLEAR!!",  "da da~", "TOP!!",
    "SCORE!!",  "...",       "GAME",     "OVER!!"};
const char *tetrisTeks2[] = {
    "~da da da~", "~",         "~",        "~da da da~", "~",
    "~",          "~drop!~",   "~",        "~",          "~line!~",
    "~",          "~",         "~clear!~", "~",          "~yeah!~",
    "(*^o^*)",    "~da da~",   "~score!~", "(*^_^*)/",   "~level up!~",
    "~",          "~faster!~", "~",        "~",          "~t-spin!~",
    "~",          "~combo!~",  "~",        "~",          "~tetris!~",
    "~",          "~",         "~clear!~", "(>o<)/",     "~da da~",
    "~top!~",     "~score!~",  "~",        "~dun dun~",  "~DUN DUN!!~"};

// =====================================================================
// === CUSTOM CHAR HAPPY BIRTHDAY ======================================
// =====================================================================
#define CHAR_NOTE1 0
#define CHAR_NOTE2 1
#define CHAR_BAR_LO 2
#define CHAR_BAR_MID 3
#define CHAR_BAR_HI 4
#define CHAR_HEART 5
#define CHAR_STAR 6
#define CHAR_CAKE 7

byte charNote1[8] = {0x04, 0x06, 0x05, 0x04, 0x04, 0x0C, 0x0C, 0x00};
byte charNote2[8] = {0x0F, 0x09, 0x09, 0x01, 0x01, 0x07, 0x07, 0x00};
byte charBarLo[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00};
byte charBarMid[8] = {0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x00};
byte charBarHi[8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00};
byte charHeart[8] = {0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00};
byte charStar[8] = {0x00, 0x04, 0x15, 0x0E, 0x1F, 0x0E, 0x15, 0x04};
byte charCake[8] = {0x04, 0x04, 0x0E, 0x1F, 0x1B, 0x1F, 0x1F, 0x00};

// =====================================================================
// === LED HELPER ======================================================
// =====================================================================
void setRgb(int r, int g, int b) {
  if (!ledOn) {
    analogWrite(PIN_LED_R, 0);
    analogWrite(PIN_LED_G, 0);
    analogWrite(PIN_LED_B, 0);
    return;
  }
  analogWrite(PIN_LED_R, r);
  analogWrite(PIN_LED_G, g);
  analogWrite(PIN_LED_B, b);
}

void updateLedFromNote(int freq) {
  if (!ledSync || !ledOn)
    return;
  if (freq == 0) {
    setRgb(0, 0, 0);
    return;
  }
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
  // Mapping frekuensi ke warna (Pelangi sederhana)
  if (freq < 300)
    setRgb(255, 0, 0); // Merah
  else if (freq < 400)
    setRgb(255, 127, 0); // Orange
  else if (freq < 500)
    setRgb(255, 255, 0); // Kuning
  else if (freq < 700)
    setRgb(0, 255, 0); // Hijau
  else if (freq < 1000)
    setRgb(0, 0, 255); // Biru
  else if (freq < 1500)
    setRgb(75, 0, 130); // Indigo
  else
    setRgb(143, 0, 255); // Ungu
}

// =====================================================================
// === LCD HELPER ======================================================
// =====================================================================
void lcdPrint(const char *b1, const char *b2) {
  char buf[17];
  lcd.setCursor(0, 0);
  snprintf(buf, sizeof(buf), "%-16s", b1);
  lcd.print(buf);
  lcd.setCursor(0, 1);
  snprintf(buf, sizeof(buf), "%-16s", b2);
  lcd.print(buf);
}

// =====================================================================
// === MAIN PLAY FUNCTION ==============================================
// =====================================================================
void playNote(int freq, int dur, float ratio = 0.85) {
  if (stopFlag)
    return;
  if (freq == 0) {
    noTone(BUZZER_PIN);
    if (ledSync)
      setRgb(0, 0, 0);
    delay(dur);
  } else {
    int onTime = (int)(dur * ratio);
    int offTime = dur - onTime;

    updateLedFromNote(freq);
    tone(BUZZER_PIN, freq, onTime);
    delay(onTime);

    noTone(BUZZER_PIN);
    if (ledSync)
      setRgb(0, 0, 0);
    delay(offTime);
  }
}

// =====================================================================
// === LAGU PLAYER =====================================================
// =====================================================================
void mainkanNadaKmA(int frekuensi, int lama) {
  if (stopFlag)
    return;
  if (frekuensi == 0) {
    noTone(BUZZER_PIN);
    if (ledSync)
      setRgb(0, 0, 0);
  } else {
    updateLedFromNote(frekuensi);
    tone(BUZZER_PIN, frekuensi, lama);
  }
  delay(lama);
  noTone(BUZZER_PIN);
  if (ledSync)
    setRgb(0, 0, 0);
  delay(50);
}

void playSong(int songId) {

  stopFlag = false;
  isPlaying = true;
  isTimer = false;
  currentSong = songId;

  // -------- FUR ELISE --------
  if (songId == 0) {
    lcdPrint("Fur Elise", "Beethoven 1810");
    delay(1800);
    for (int i = 0; i < furEliseTotal && !stopFlag; i++) {
      lcdPrint(furEliseTeks1[i], furEliseTeks2[i]);
      playNote(furEliseMelodi[i], furEliseDurasi[i]);
    }
    if (!stopFlag)
      lcdPrint("Fur Elise Selesai", "(*^_^*)");
  }

  // -------- HAPPY BIRTHDAY --------
  else if (songId == 1) {
    lcd.createChar(CHAR_NOTE1, charNote1);
    lcd.createChar(CHAR_NOTE2, charNote2);
    lcd.createChar(CHAR_BAR_LO, charBarLo);
    lcd.createChar(CHAR_BAR_MID, charBarMid);
    lcd.createChar(CHAR_BAR_HI, charBarHi);
    lcd.createChar(CHAR_HEART, charHeart);
    lcd.createChar(CHAR_STAR, charStar);
    lcd.createChar(CHAR_CAKE, charCake);
    lcdPrint("Happy Birthday!", "Siap...");
    delay(2000);
    for (int i = 0; i < happyBirthTotal && !stopFlag; i++) {
      // baris 2 dengan custom char
      char buf[17];
      lcd.setCursor(0, 0);
      snprintf(buf, sizeof(buf), "%-16s", happyBirthTeks1[i]);
      lcd.print(buf);
      lcd.setCursor(0, 1);
      int freq = happyBirthMelodi[i];
      byte barChar = (freq <= 988)    ? CHAR_BAR_LO
                     : (freq <= 1319) ? CHAR_BAR_MID
                                      : CHAR_BAR_HI;
      byte notChar = (i % 2 == 0) ? CHAR_NOTE1 : CHAR_NOTE2;
      byte ikonK = (i < 6)    ? CHAR_HEART
                   : (i < 12) ? CHAR_STAR
                   : (i < 19) ? CHAR_CAKE
                              : CHAR_HEART;
      lcd.write(notChar);
      lcd.print(" ");
      lcd.write(barChar);
      lcd.write(barChar);
      lcd.write(barChar);
      lcd.print(" ");
      lcd.write(ikonK);
      lcd.print(" ");
      lcd.write(notChar);
      lcd.print("        ");
      playNote(freq, happyBirthDurasi[i], 1.0);
      delay(60);
    }
    if (!stopFlag)
      lcdPrint("Happy Birthday!", "Selesai!");
  }

  // -------- ITS NOT LIKE I LIKE YOU --------
  else if (songId == 2) {
    lcdPrint("ESP32 Music", "Siap...");
    delay(1500);
    for (int i = 0; i < itsNotTotal && !stopFlag; i++) {
      lcdPrint(itsNotTeks1[i], itsNotTeks2[i]);
      playNote(itsNotMelodi[i], itsNotDurasi[i]);
      delay(50);
    }
    if (!stopFlag)
      lcdPrint("Selesai!", ">///<");
  }
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
  // -------- MAYOR MINOR --------
  else if (songId == 3) {
    lcdPrint("Mayor Minor", "Scale Demo");
    delay(1500);
    for (int i = 0; i < mayorMinorTotal && !stopFlag; i++) {
      lcdPrint(mayorMinorTeks1[i], mayorMinorTeks2[i]);
      playNote(mayorMinorMelodi[i], mayorMinorDurasi[i]);
      delay(50);
    }
    if (!stopFlag)
      lcdPrint("Scale Selesai", "(*^_^*)");
  }

  // -------- NOKIA RINGTONE --------
  else if (songId == 4) {
    lcdPrint("Nokia Ringtone", "Gran Vals 1902");
    delay(1500);
    for (int i = 0; i < nokiaTotal && !stopFlag; i++) {
      lcdPrint(nokiaTeks1[i], nokiaTeks2[i]);
      playNote(nokiaMelodi[i], nokiaDurasi[i]);
      delay(50);
    }
    if (!stopFlag)
      lcdPrint("Nokia Selesai", "(^_^)/");
  }

  // -------- SUPER MARIO --------
  else if (songId == 5) {
    lcdPrint("Super Mario Bros", "Nintendo 1985");
    delay(1800);
    for (int i = 0; i < marioTotal && !stopFlag; i++) {
      lcdPrint(marioTeks1[i], marioTeks2[i]);
      playNote(marioMelodi[i], marioDurasi[i]);
      delay(40);
    }
    if (!stopFlag)
      lcdPrint("GAME CLEAR!", "(*^o^*)/");
  }

  // -------- SWEET LITTLE BUMBLEBEE --------
  else if (songId == 6) {
    lcdPrint("Sweet Bumblebee", "Siap...");
    delay(1500);
    for (int i = 0; i < bumblebeeTotal && !stopFlag; i++) {
      lcdPrint(bumblebeeTeks1[i], bumblebeeTeks2[i]);
      playNote(bumblebeeMelodi[i], bumblebeeDurasi[i]);
      delay(50);
    }
    if (!stopFlag)
      lcdPrint("Selesai!", "La da da~");
  }

  // -------- KISS ME AGAIN --------
  else if (songId == 7) {
    lcdPrint("Kiss Me Again", "Siap...");
    delay(1500);

    // Data lagu (diambil dari Kiss_Me_Again.cpp)
    const int totalNadaKmA = 97;
    const int melodiKmA[] = {
        FA2,  FA2, MI2,  FA2, LA2,  FA2,  DO2, LA1, DO2, MI2, FA2,  FA2, DO2,
        LAm1, LA1, DIAM,

        LA2,  FA2, DIAM, DO2, LA1,  DO2,  MI2, FA2, FA2, DO2, LAm1, LA1, DIAM,

        LA2,  FA2, DIAM, DO2, LA1,  DO2,  MI2, FA2, FA2, DO2, LAm1, LA1, DIAM,

        LA2,  FA2, LA2,  FA2, SOL2, SOL2, RE2, MI2,

        LA2,  FA2, DIAM, DO2, LA1,  DO2,  MI2, FA2, FA2, DO2, LAm1, LA1, DIAM,

        LA2,  FA2, DIAM, DO2, LA1,  DO2,  MI2, FA2, FA2, DO2, LAm1, LA1, DIAM,

        LA2,  FA2, DIAM, DO2, LA1,  DO2,  MI2, FA2, FA2, DO2, LAm1, LA1, DIAM,

        LA2,  FA2, LA2,  FA2, SOL2, SOL2, RE2, MI2};

    const int durasiKmA[] = {
        200, 200, 200, 200, 400, 400, 200, 200, 200, 200, 400, 400, 200,
        200, 200, 200,

        400, 400, 100, 200, 200, 200, 200, 400, 400, 200, 200, 200, 200,

        400, 400, 100, 200, 200, 200, 200, 400, 400, 200, 200, 200, 200,

        400, 400, 400, 400, 300, 300, 200, 800,

        400, 400, 100, 200, 200, 200, 200, 400, 400, 200, 200, 200, 200,

        400, 400, 100, 200, 200, 200, 200, 400, 400, 200, 200, 200, 200,

        400, 400, 100, 200, 200, 200, 200, 400, 400, 200, 200, 200, 200,

        400, 400, 400, 400, 300, 300, 200, 800};

    const char *teks1KmA[] = {"I",
                              "I Re-",
                              "I Remem-",
                              "I Remember",
                              "I Remember",
                              "I Remember",
                              "May-",
                              "Maybe",
                              "Maybe was",
                              "Maybe was the",
                              "Maybe was the",
                              "Maybe was the",
                              "Of",
                              "Of my",
                              "Of my life",
                              "Of my life",
                              "You",
                              "You came",
                              "You came",
                              "To",
                              "To my",
                              "To my heart",
                              "To my heart",
                              "To my heart",
                              "To my heart",
                              "Open",
                              "Open to",
                              "Open to you",
                              "Open to you",
                              "I",
                              "I feel",
                              "I feel",
                              "Warm",
                              "Warm be-",
                              "Warm behind",
                              "Warm behind you",
                              "Take",
                              "Take my",
                              "Take my hand",
                              "Take my hand",
                              "Take my hand",
                              "Take my hand",
                              "Kiss",
                              "Kiss me",
                              "Kiss",
                              "Kiss me",
                              "Kiss",
                              "Kiss me",
                              "Kiss me a-",
                              "Kiss me again",
                              "I",
                              "I feel",
                              "I feel",
                              "You",
                              "You re-",
                              "You remem-",
                              "You remember",
                              "You remember",
                              "You remember",
                              "You remember",
                              "To",
                              "To love",
                              "To love",
                              "Yes",
                              "Yes girl!",
                              "Yes girl!",
                              "Yes!",
                              "Yes! I",
                              "Yes! I love",
                              "Yes! I love",
                              "Yes! I love",
                              "Yes! I love",
                              "Me",
                              "Me at",
                              "Me at all",
                              "",
                              "Just touch",
                              "Just touch my",
                              "Just touch my",
                              "Lips",
                              "Lips and",
                              "Lips and got",
                              "Lips and got",
                              "Lips and got",
                              "Lips and got",
                              "You still",
                              "You still",
                              "You still",
                              "You still",
                              "Kiss",
                              "Kiss you",
                              "Kiss",
                              "Kiss you",
                              "Kiss",
                              "Kiss you",
                              "Kiss you a-",
                              "Kiss you again"};

    const char *teks2KmA[] = {"",
                              "",
                              "",
                              "",
                              "One",
                              "One day",
                              "",
                              "",
                              "",
                              "",
                              "Best",
                              "Best day",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "My",
                              "My eyes",
                              "My eyes wide",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "Ins-",
                              "Inside",
                              "Inside",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "><",
                              ">///<",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "One",
                              "One day",
                              "One day I start",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "You",
                              "You more",
                              "You more than",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "This",
                              "This pic-",
                              "This picture",
                              "",
                              "Remem-",
                              "Remember",
                              "Remember",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "",
                              "><",
                              ">///<"};

    for (int i = 0; i < totalNadaKmA && !stopFlag; i++) {
      lcdPrint(teks1KmA[i], teks2KmA[i]);
      mainkanNadaKmA(melodiKmA[i], durasiKmA[i]);
    }
    if (!stopFlag)
      lcdPrint("Selesai!", "EN untuk ulangi");
  }

  // -------- TETRIS THEME --------
  else if (songId == 8) {

    lcdPrint("Tetris Theme", "Korobeiniki");
    delay(1800);
    for (int i = 0; i < tetrisTotal && !stopFlag; i++) {
      lcdPrint(tetrisTeks1[i], tetrisTeks2[i]);
      int dur = tetrisDurasi[i];
      int onT = (int)(dur * 0.85);
      int offT = dur - onT;
      if (!stopFlag) {
        if (tetrisMelodi[i] == DIAM) {
          noTone(BUZZER_PIN);
          if (ledSync)
            setRgb(0, 0, 0);
          delay(dur);
        } else {
          updateLedFromNote(tetrisMelodi[i]);
          tone(BUZZER_PIN, tetrisMelodi[i], onT);
          delay(onT);
          noTone(BUZZER_PIN);
          if (ledSync)
            setRgb(0, 0, 0);
          delay(offT);
        }
      }
    }
    if (!stopFlag)
      lcdPrint("GAME CLEAR! :D", "(*^_^*)/");
  }

  noTone(BUZZER_PIN);
  isPlaying = false;
  currentSong = -1;
}
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
// =====================================================================
// === COUNTDOWN TIMER =================================================
// =====================================================================
int beepIntervalMs[] = {900, 850, 800, 720, 640, 560, 480, 400,
                        330, 270, 220, 180, 150, 120, 100};

void runTimer(int totalSec) {
  stopFlag = false;
  isPlaying = false;
  isTimer = true;

  char buf1[17], buf2[17];
  int mm = totalSec / 60;
  int ss = totalSec % 60;
  snprintf(buf1, sizeof(buf1), "  Timer: %02d:%02d  ", mm, ss);
  lcdPrint(buf1, "  Mulai hitung  ");
  delay(1500);

  for (int detik = totalSec; detik >= 1 && !stopFlag; detik--) {
    int m = detik / 60;
    int s = detik % 60;
    snprintf(buf1, sizeof(buf1), "  Hitung Mundur ");
    snprintf(buf2, sizeof(buf2), "     %02d:%02d      ", m, s);
    lcdPrint(buf1, buf2);

    // Led menyala tiap detik
    if (ledOn && ledSync)
      setRgb(255, 255, 255); // Putih saat timer
    delay(100);
    if (ledOn && ledSync)
      setRgb(0, 0, 0);

    // Beep: makin cepat di 15 detik terakhir
    if (detik <= 15 && !stopFlag) {
      int idx = 15 - detik;
      int interval = beepIntervalMs[idx];
      int beepLama = 50;
      int freq = 880 + (idx * 80);
      if (freq > 2000)
        freq = 2000;
      unsigned long mulai = millis();
      while (millis() - mulai < 900 &&
             !stopFlag) { // 900 karena sudah delay 100 di atas
        unsigned long sisa = 900 - (millis() - mulai);
        if (sisa < (unsigned long)beepLama)
          break;

        updateLedFromNote(freq);
        tone(BUZZER_PIN, freq, beepLama);
        delay(beepLama);
        noTone(BUZZER_PIN);
        if (ledSync)
          setRgb(0, 0, 0);

        unsigned long jedaAktual = interval - beepLama;
        unsigned long sisaSetelah = 900 - (millis() - mulai);
        if (jedaAktual > sisaSetelah)
          break;
        delay(jedaAktual);
      }
    } else if (!stopFlag) {
      // Delay sisa detik
      unsigned long t = millis();
      while (millis() - t < 900 && !stopFlag)
        delay(10);
    }
  }

  if (!stopFlag) {
    lcdPrint("  Hitung Mundur ", "     00:00      ");

    // Berkedip berkali-kali saat selesai
    for (int b = 0; b < 10; b++) {
      setRgb(255, 0, 0);
      delay(100);
      setRgb(0, 255, 0);
      delay(100);
      setRgb(0, 0, 255);
      delay(100);
    }
    setRgb(0, 0, 0);

    delay(300);
    lcdPrint(" ---Time out--- ", "");

    int nada[] = {C5, E5, G5, C6, 1319, C6, 1319};
    int lama[] = {120, 120, 120, 200, 400, 150, 700};
    for (int i = 0; i < 7 && !stopFlag; i++) {
      updateLedFromNote(nada[i]);
      tone(BUZZER_PIN, nada[i], lama[i]);
      delay(lama[i] + 40);
      noTone(BUZZER_PIN);
      if (ledSync)
        setRgb(0, 0, 0);
      delay(20);
    }

    // Relay ON setelah nada ---Time out--- selesai
    if (!stopFlag) {
      digitalWrite(PIN_RELAY, HIGH);
      relayState = true;
      delay(3000); // Menyala selama 3 detik
      digitalWrite(PIN_RELAY, LOW);
      relayState = false;
    }
  }

  noTone(BUZZER_PIN);
  isTimer = false;
}

// =====================================================================
// === STATUS JSON =====================================================
// =====================================================================
String getStatusJson() {
  String s = "{";
  s += "\"playing\":" + String(isPlaying ? "true" : "false") + ",";
  s += "\"timer\":" + String(isTimer ? "true" : "false") + ",";
  s += "\"song\":" + String(currentSong) + ",";
  s += "\"timerSet\":" + String(timerSeconds) + ",";
  s += "\"ledOn\":" + String(ledOn ? "true" : "false") + ",";
  s += "\"ledSync\":" + String(ledSync ? "true" : "false") + ",";
  s += "\"ledR\":" + String(ledR) + ",";
  s += "\"ledG\":" + String(ledG) + ",";
  s += "\"ledB\":" + String(ledB) + ",";
  s += "\"relay\":" + String(relayState ? "true" : "false");
  s += "}";
  return s;
}

// =====================================================================
// === HTML WEB UI =====================================================
// =====================================================================
const char *HTML_PAGE = R"rawhtml(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 IoT Music &amp; Timer</title>
<style>
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body {
    font-family: 'Segoe UI', Arial, sans-serif;
    background: linear-gradient(135deg, #1a1a2e 0%, #16213e 50%, #0f3460 100%);
    min-height: 100vh;
    color: #eee;
  }
  h1 {
    text-align: center;
    padding: 24px 16px 8px;
    font-size: 1.8em;
    color: #e94560;
    text-shadow: 0 0 20px #e9456066;
    letter-spacing: 2px;
  }
  .subtitle {
    text-align: center;
    color: #a0a0c0;
    font-size: 0.85em;
    margin-bottom: 24px;
  }
  .status-bar {
    margin: 0 auto 20px;
    max-width: 500px;
    background: rgba(255,255,255,0.07);
    border: 1px solid rgba(233,69,96,0.3);
    border-radius: 12px;
    padding: 12px 20px;
    display: flex;
    align-items: center;
    gap: 10px;
    font-size: 0.9em;
  }
  .dot {
    width: 10px; height: 10px;
    border-radius: 50%;
    background: #444;
    flex-shrink: 0;
    transition: background 0.3s;
  }
  .dot.active { background: #4ade80; box-shadow: 0 0 8px #4ade80; }
  .section {
    max-width: 540px;
    margin: 0 auto 24px;
    padding: 0 12px;
  }
  .section-title {
    font-size: 1em;
    font-weight: 700;
    color: #a78bfa;
    margin-bottom: 12px;
    letter-spacing: 1px;
    display: flex;
    align-items: center;
    gap: 8px;
  }
  .songs-grid {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 10px;
  }
  .song-btn {
    background: rgba(255,255,255,0.06);
    border: 1px solid rgba(167,139,250,0.25);
    border-radius: 12px;
    padding: 14px 12px;
    cursor: pointer;
    text-align: left;
    transition: all 0.2s;
    color: #eee;
  }
  .song-btn:hover {
    background: rgba(167,139,250,0.15);
    border-color: #a78bfa;
    transform: translateY(-2px);
  }
  .song-btn.active {
    background: rgba(233,69,96,0.2);
    border-color: #e94560;
    box-shadow: 0 0 14px rgba(233,69,96,0.3);
  }
  .song-btn .snum {
    font-size: 0.65em;
    color: #a78bfa;
    font-weight: 700;
    letter-spacing: 1px;
  }
  .song-btn .sname {
    font-size: 0.9em;
    font-weight: 600;
    margin-top: 2px;
    line-height: 1.3;
  }
  .song-btn .sdesc {
    font-size: 0.72em;
    color: #888;
    margin-top: 3px;
  }
  /* Timer Section */
  .timer-box {
    background: rgba(255,255,255,0.06);
    border: 1px solid rgba(74,222,128,0.25);
    border-radius: 16px;
    padding: 20px;
  }
  .timer-display {
    text-align: center;
    font-size: 3em;
    font-weight: 700;
    font-family: monospace;
    color: #4ade80;
    text-shadow: 0 0 20px #4ade8055;
    margin: 8px 0 16px;
    letter-spacing: 4px;
  }
  .timer-controls {
    display: flex;
    gap: 8px;
    flex-wrap: wrap;
    margin-bottom: 14px;
  }
  .preset-btn {
    background: rgba(74,222,128,0.1);
    border: 1px solid rgba(74,222,128,0.3);
    color: #4ade80;
    padding: 7px 14px;
    border-radius: 8px;
    cursor: pointer;
    font-size: 0.82em;
    font-weight: 600;
    transition: all 0.2s;
  }
  .preset-btn:hover {
    background: rgba(74,222,128,0.25);
  }
  .custom-timer {
    display: flex;
    gap: 8px;
    align-items: center;
    margin-bottom: 14px;
  }
  .custom-timer label { font-size: 0.82em; color: #888; white-space: nowrap; }
  .custom-timer input {
    flex: 1;
    background: rgba(255,255,255,0.08);
    border: 1px solid #555;
    border-radius: 8px;
    padding: 8px 12px;
    color: #eee;
    font-size: 0.9em;
    outline: none;
    min-width: 0;
  }
  .custom-timer input:focus { border-color: #4ade80; }
  /* Tombol aksi */
  .action-row {
    display: flex;
    gap: 10px;
    margin-top: 4px;
  }
  .btn-play {
    flex: 1;
    padding: 13px;
    border-radius: 10px;
    border: none;
    font-size: 0.95em;
    font-weight: 700;
    cursor: pointer;
    transition: all 0.2s;
    letter-spacing: 0.5px;
  }
  .btn-green {
    background: linear-gradient(135deg, #22c55e, #16a34a);
    color: #fff;
    box-shadow: 0 4px 14px rgba(34,197,94,0.35);
  }
  .btn-green:hover { transform: translateY(-2px); box-shadow: 0 6px 20px rgba(34,197,94,0.5); }
  .btn-red {
    background: linear-gradient(135deg, #e94560, #c0392b);
    color: #fff;
    box-shadow: 0 4px 14px rgba(233,69,96,0.35);
  }
  .btn-red:hover { transform: translateY(-2px); }
  .toast {
    position: fixed;
    bottom: 20px;
    left: 50%;
    transform: translateX(-50%);
    background: rgba(0,0,0,0.85);
    color: #4ade80;
    padding: 10px 22px;
    border-radius: 30px;
    font-size: 0.88em;
    opacity: 0;
    pointer-events: none;
    transition: opacity 0.3s;
    border: 1px solid #4ade8044;
    white-space: nowrap;
  }
  .toast.show { opacity: 1; }
  /* LED Control */
  .led-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; margin-top: 10px; }
  .led-card {
    background: rgba(255,255,255,0.06);
    border: 1px solid rgba(233,69,96,0.3);
    border-radius: 12px; padding: 15px;
  }
  .switch-container { display: flex; align-items: center; justify-content: space-between; margin-bottom: 10px; }
  input[type="color"] {
    width: 100%; height: 40px; border: none; border-radius: 8px; background: none; cursor: pointer;
  }
  .btn-toggle {
    padding: 8px 12px; border-radius: 8px; border: 1px solid rgba(255,255,255,0.2);
    background: rgba(255,255,255,0.1); color: #eee; cursor: pointer; font-size: 0.85em; font-weight: 600;
  }
  .btn-toggle.active { background: #e94560; border-color: #e94560; }
  .btn-relay { background: rgba(74, 222, 128, 0.1); border-color: rgba(74, 222, 128, 0.3); color: #4ade80; }
  .btn-relay.active { background: #22c55e; border-color: #22c55e; color: #fff; }
  @media (max-width: 400px) {
    .songs-grid { grid-template-columns: 1fr; }
    .timer-display { font-size: 2.4em; }
    .led-grid { grid-template-columns: 1fr; }
  }
</style>
</head>
<body>
<h1>&#127925; ESP32 IoT Control</h1>
<p class="subtitle">WiFi Music Player &amp; Timer via Web</p>

<div class="status-bar">
  <div class="dot" id="statusDot"></div>
  <span id="statusText">Mengecek status...</span>
</div>

<!-- MUSIK -->
<div class="section">
  <div class="section-title">&#127925; Pilih Musik</div>
  <div class="songs-grid">
    <button class="song-btn" id="sb0" onclick="playSong(0)">
      <div class="snum">LAGU 1</div>
      <div class="sname">Fur Elise</div>
      <div class="sdesc">Beethoven, 1810</div>
    </button>
    <button class="song-btn" id="sb1" onclick="playSong(1)">
      <div class="snum">LAGU 2</div>
      <div class="sname">Happy Birthday</div>
      <div class="sdesc">Klasik semua zaman</div>
    </button>
    <button class="song-btn" id="sb2" onclick="playSong(2)">
      <div class="snum">LAGU 3</div>
      <div class="sname">Its Not Like I Like You</div>
      <div class="sdesc">Tsundere Anthem</div>
    </button>
    <button class="song-btn" id="sb3" onclick="playSong(3)">
      <div class="snum">LAGU 4</div>
      <div class="sname">Mayor Minor Scale</div>
      <div class="sdesc">Skala tangga nada</div>
    </button>
    <button class="song-btn" id="sb4" onclick="playSong(4)">
      <div class="snum">LAGU 5</div>
      <div class="sname">Nokia Ringtone</div>
      <div class="sdesc">Gran Vals, Tarrega 1902</div>
    </button>
    <button class="song-btn" id="sb5" onclick="playSong(5)">
      <div class="snum">LAGU 6</div>
      <div class="sname">Super Mario Theme</div>
      <div class="sdesc">Nintendo, 1985</div>
    </button>
    <button class="song-btn" id="sb6" onclick="playSong(6)">
      <div class="snum">LAGU 7</div>
      <div class="sname">Sweet Little Bumblebee</div>
      <div class="sdesc">Sweet melody</div>
    </button>
    <button class="song-btn" id="sb7" onclick="playSong(7)">
      <div class="snum">LAGU 8</div>
      <div class="sname">Kiss Me Again</div>
      <div class="sdesc">Sweet love song</div>
    </button>
    <button class="song-btn" id="sb8" onclick="playSong(8)">
      <div class="snum">LAGU 9</div>
      <div class="sname">Tetris Theme</div>
      <div class="sdesc">Korobeiniki</div>
    </button>
  </div>
  <div class="action-row" style="margin-top:12px">
    <button class="btn-play btn-red" onclick="stopAll()">&#9646;&#9646; Stop Musik</button>
  </div>
</div>

<!-- LCD CONTROL -->
<div class="section">
  <div class="section-title">&#128221; LCD Control</div>
  <div class="timer-box" style="border-color: rgba(167, 139, 250, 0.25);">
    <div style="font-size:0.85em;color:#888;margin-bottom:12px">Tampilkan teks kustom di layar LCD 16x2</div>
    <div class="custom-timer">
      <label style="width: 60px">Baris 1:</label>
      <input type="text" id="lcdL1" placeholder="Maks 16 karakter" maxlength="16">
    </div>
    <div class="custom-timer">
      <label style="width: 60px">Baris 2:</label>
      <input type="text" id="lcdL2" placeholder="Maks 16 karakter" maxlength="16">
    </div>
    <div class="action-row">
      <button class="btn-play btn-green" onclick="setLCD()">&#128187; Update LCD</button>
      <button class="btn-play btn-red" onclick="stopAll()">&#9646;&#9646; Reset</button>
    </div>
  </div>
</div>

<!-- TIMER -->
<div class="section">
  <div class="section-title">&#9201; Countdown Timer</div>
  <div class="timer-box">
    <div class="timer-display" id="timerDisp">00:15</div>
    <div style="font-size:0.8em;color:#888;text-align:center;margin-bottom:12px">Durasi timer yang akan dijalankan</div>
    <div class="timer-controls">
      <button class="preset-btn" onclick="setTimer(10)">10 dtk</button>
      <button class="preset-btn" onclick="setTimer(15)">15 dtk</button>
      <button class="preset-btn" onclick="setTimer(30)">30 dtk</button>
      <button class="preset-btn" onclick="setTimer(60)">1 menit</button>
      <button class="preset-btn" onclick="setTimer(120)">2 menit</button>
      <button class="preset-btn" onclick="setTimer(300)">5 menit</button>
      <button class="preset-btn" onclick="setTimer(600)">10 menit</button>
    </div>
    <div class="custom-timer">
      <label>Custom (detik):</label>
      <input type="number" id="customSec" placeholder="mis: 45" min="1" max="3600" oninput="previewCustom()">
    </div>
    <div class="action-row">
      <button class="btn-play btn-green" onclick="startTimer()">&#9654; Mulai Timer</button>
      <button class="btn-play btn-red" onclick="stopAll()">&#9646;&#9646; Stop</button>
    </div>
  </div>
</div>

<!-- LED CONTROL -->
<div class="section">
  <div class="section-title">&#128161; RGB LED Control</div>
  <div class="led-grid">
    <div class="led-card">
      <div class="switch-container">
        <span>Power LED</span>
        <button id="btnLedPower" class="btn-toggle" onclick="toggleLedPower()">OFF</button>
      </div>
      <div class="switch-container">
        <span>Sync Musik</span>
        <button id="btnLedSync" class="btn-toggle" onclick="toggleLedSync()">OFF</button>
      </div>
    </div>
    <div class="led-card">
      <div style="font-size: 0.8em; color: #888; margin-bottom: 5px;">Warna Manual</div>
      <input type="color" id="ledColor" onchange="updateLedColor()" value="#ff0000">
    </div>
  </div>
</div>

<!-- RELAY CONTROL -->
<div class="section">
  <div class="section-title">&#128268; Relay Control (D27)</div>
  <div class="timer-box" style="border-color: rgba(74, 222, 128, 0.3);">
    <div style="display: flex; align-items: center; justify-content: space-between;">
      <div>
        <div style="font-size: 0.9em; font-weight: 600;">Status Relay</div>
        <div style="font-size: 0.75em; color: #888;">Manual On/Off pin D27</div>
      </div>
      <button id="btnRelay" class="btn-play btn-relay" style="flex: 0 0 120px;" onclick="toggleRelay()">OFF</button>
    </div>
  </div>
</div>

<div class="toast" id="toast"></div>

<script>
  let currentSeconds = 15;

  function fmt(s) {
    let m = Math.floor(s / 60);
    let sec = s % 60;
    return (m < 10 ? '0' : '') + m + ':' + (sec < 10 ? '0' : '') + sec;
  }

  function setTimer(s) {
    currentSeconds = s;
    document.getElementById('timerDisp').textContent = fmt(s);
    document.getElementById('customSec').value = '';
    showToast('Timer diset ke ' + fmt(s));
  }

  function previewCustom() {
    let v = parseInt(document.getElementById('customSec').value);
    if (v > 0 && v <= 3600) {
      currentSeconds = v;
      document.getElementById('timerDisp').textContent = fmt(v);
    }
  }

  function playSong(id) {
    showToast('Memulai lagu...');
    fetch('/play?song=' + id).then(r => r.text()).then(t => {
      showToast(t);
      updateStatus();
    });
  }

  function startTimer() {
    let v = parseInt(document.getElementById('customSec').value);
    if (v > 0 && v <= 3600) currentSeconds = v;
    showToast('Memulai timer ' + fmt(currentSeconds) + '...');
    fetch('/timer?seconds=' + currentSeconds).then(r => r.text()).then(t => {
      showToast(t);
      updateStatus();
    });
  }

  function stopAll() {
    fetch('/stop').then(r => r.text()).then(t => {
      showToast(t);
      updateStatus();
    });
  }

  function setLCD() {
    let l1 = document.getElementById('lcdL1').value;
    let l2 = document.getElementById('lcdL2').value;
    showToast('Mengirim ke LCD...');
    fetch('/setlcd?l1=' + encodeURIComponent(l1) + '&l2=' + encodeURIComponent(l2))
      .then(r => r.text())
      .then(t => {
        showToast(t);
        updateStatus();
      });
  }

  let ledOn = true;
  let ledSync = true;
  let relayOn = false;

  function toggleLedPower() {
    ledOn = !ledOn;
    sendLedCmd();
  }

  function toggleLedSync() {
    ledSync = !ledSync;
    sendLedCmd();
  }

  function updateLedColor() {
    sendLedCmd();
  }

  function sendLedCmd() {
    let color = document.getElementById('ledColor').value;
    let r = parseInt(color.substr(1,2), 16);
    let g = parseInt(color.substr(3,2), 16);
    let b = parseInt(color.substr(5,2), 16);
    fetch(`/setledrgb?r=${r}&g=${g}&b=${b}&on=${ledOn?1:0}&sync=${ledSync?1:0}`)
      .then(r => r.text()).then(t => {
        showToast('LED: ' + t);
        updateStatus();
      });
  }

  function toggleRelay() {
    let newState = relayOn ? 0 : 1;
    fetch('/setrelay?state=' + newState).then(r => r.text()).then(t => {
      showToast('Relay: ' + t);
      updateStatus();
    });
  }

  function showToast(msg) {
    let el = document.getElementById('toast');
    el.textContent = msg;
    el.classList.add('show');
    setTimeout(() => el.classList.remove('show'), 2500);
  }

  function updateStatus() {
    fetch('/status').then(r => r.json()).then(d => {
      // Update LED controls
      ledOn = d.ledOn;
      ledSync = d.ledSync;
      relayOn = d.relay;

      let btnP = document.getElementById('btnLedPower');
      btnP.textContent = d.ledOn ? 'ON' : 'OFF';
      btnP.classList.toggle('active', d.ledOn);

      let btnS = document.getElementById('btnLedSync');
      btnS.textContent = d.ledSync ? 'SYNC ON' : 'SYNC OFF';
      btnS.classList.toggle('active', d.ledSync);

      let btnR = document.getElementById('btnRelay');
      btnR.textContent = d.relay ? 'RELAY ON' : 'RELAY OFF';
      btnR.classList.toggle('active', d.relay);

      let hex = "#" + ((1 << 24) + (d.ledR << 16) + (d.ledG << 8) + d.ledB).toString(16).slice(1);
      document.getElementById('ledColor').value = hex;

      let dot  = document.getElementById('statusDot');
      let txt  = document.getElementById('statusText');
      let names = [
        'Fur Elise',
        'Happy Birthday',
        'Its Not Like I Like You',
        'Mayor Minor Scale',
        'Nokia Ringtone',
        'Super Mario Theme',
        'Sweet Little Bumblebee',
        'Kiss Me Again',
        'Tetris Theme'
      ];

      dot.className = 'dot';
      if (d.playing || d.timer) dot.classList.add('active');

      if (d.playing && d.song >= 0) {
        txt.textContent = '\u25B6 Memutar: ' + (names[d.song] || 'Unknown');
      } else if (d.timer) {
        txt.textContent = '\u23F1 Timer sedang berjalan...';
      } else {
        txt.textContent = 'Siap \u2014 pilih musik atau jalankan timer';
      }

      for (let i = 0; i <= 8; i++) {
        let btn = document.getElementById('sb' + i);
        if (btn) {
          btn.classList.toggle('active', d.playing && d.song === i);
        }
      }
    }).catch(() => {
      document.getElementById('statusText').textContent = 'Tidak dapat terhubung ke ESP32';
    });
  }

  setInterval(updateStatus, 2000);
  updateStatus();
</script>
</body>
</html>
)rawhtml";
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
// =====================================================================
// === WEB SERVER HANDLER ==============================================
// =====================================================================
TaskHandle_t musicTaskHandle = NULL;

struct TaskParams {
  int mode;  // 0 = song, 1 = timer
  int value; // song id atau timer seconds
};
TaskParams taskParams;

void musicTask(void *pvParams) {
  TaskParams *p = (TaskParams *)pvParams;
  if (p->mode == 0) {
    playSong(p->value);
  } else {
    runTimer(p->value);
  }
  musicTaskHandle = NULL;
  vTaskDelete(NULL);
}

void startMusicTask(int mode, int value) {
  // Hentikan task sebelumnya jika ada
  if (musicTaskHandle != NULL) {
    stopFlag = true;

    // Tunggu maksimal 1000ms agar task selesai sendiri (keluar dari delay/loop)
    int waitCounter = 0;
    while (musicTaskHandle != NULL && waitCounter < 20) {
      delay(50);
      waitCounter++;
    }

    // Jika task masih nyangkut, force delete (hindari NULL pointer delete)
    if (musicTaskHandle != NULL) {
      TaskHandle_t tempHandle = musicTaskHandle;
      musicTaskHandle = NULL;
      vTaskDelete(tempHandle);
    }

    noTone(BUZZER_PIN);
    isPlaying = false;
    isTimer = false;
  }
  taskParams.mode = mode;
  taskParams.value = value;
  stopFlag = false;
  xTaskCreatePinnedToCore(musicTask, "music", 8192, &taskParams, 1,
                          &musicTaskHandle, 1);
}

void handleRoot() { server.send(200, "text/html", HTML_PAGE); }

void handlePlay() {
  if (!server.hasArg("song")) {
    server.send(400, "text/plain", "Parameter 'song' diperlukan");
    return;
  }
  int songId = server.arg("song").toInt();
  if (songId < 0 || songId > 8) {
    server.send(400, "text/plain", "Nomor lagu 0-7");

    return;
  }
  String names[] = {"Fur Elise",
                    "Happy Birthday",
                    "Its Not Like I Like You",
                    "Mayor Minor Scale",
                    "Nokia Ringtone",
                    "Super Mario Theme",
                    "Sweet Little Bumblebee",
                    "Kiss Me Again",
                    "Tetris Theme"};
  startMusicTask(0, songId);
  server.send(200, "text/plain", "Memutar: " + names[songId]);
}

void handleTimer() {
  if (!server.hasArg("seconds")) {
    server.send(400, "text/plain", "Parameter 'seconds' diperlukan");
    return;
  }
  int sec = server.arg("seconds").toInt();
  if (sec < 1 || sec > 3600) {
    server.send(400, "text/plain", "Detik harus 1-3600");
    return;
  }
  timerSeconds = sec;
  startMusicTask(1, sec);
  int m = sec / 60, s = sec % 60;
  char buf[40];
  snprintf(buf, sizeof(buf), "Timer dimulai: %02d:%02d", m, s);
  server.send(200, "text/plain", String(buf));
}

void handleStop() {
  stopFlag = true;
  delay(100);
  noTone(BUZZER_PIN);
  isPlaying = false;
  isTimer = false;
  lcdPrint("  ESP32 IoT     ", "  Web Ready...  ");
  server.send(200, "text/plain", "Dihentikan");
}

void handleStatus() { server.send(200, "application/json", getStatusJson()); }

void handleSetLCD() {
  String l1 = server.hasArg("l1") ? server.arg("l1") : "";
  String l2 = server.hasArg("l2") ? server.arg("l2") : "";

  // Hentikan aktivitas lain agar tidak menimpa LCD
  stopFlag = true;
  delay(150);
  noTone(BUZZER_PIN);
  isPlaying = false;
  isTimer = false;

  lcdPrint(l1.c_str(), l2.c_str());
  server.send(200, "text/plain", "LCD Terupdate!");
}
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
void handleSetLedRgb() {
  if (server.hasArg("on"))
    ledOn = server.arg("on") == "1";
  if (server.hasArg("sync"))
    ledSync = server.arg("sync") == "1";
  if (server.hasArg("r"))
    ledR = server.arg("r").toInt();
  if (server.hasArg("g"))
    ledG = server.arg("g").toInt();
  if (server.hasArg("b"))
    ledB = server.arg("b").toInt();

  if (ledOn) {
    // Jika sedang tidak memutar musik/timer, tampilkan warna manual
    // meskipun Sync aktif (agar user tahu LED sudah nyala)
    if (!isPlaying && !isTimer) {
      setRgb(ledR, ledG, ledB);
    } else {
      // Jika sedang main musik
      if (!ledSync)
        setRgb(ledR, ledG, ledB);
      // Jika sync ON, nada selanjutnya yang akan mengatur
    }
  } else {
    setRgb(0, 0, 0);
  }
  server.send(200, "text/plain", "OK");
}

void handleSetRelay() {
  if (server.hasArg("state")) {
    relayState = server.arg("state") == "1";
    digitalWrite(PIN_RELAY, relayState ? HIGH : LOW);
  }
  server.send(200, "text/plain", relayState ? "ON" : "OFF");
}

// =====================================================================
// === SETUP ===========================================================
// =====================================================================
void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();
  lcdPrint("  ESP32 IoT     ", "  Menghubungkan ");

  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);

  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);

  // FIX LED REDUP (ESP32 Core 3.x)
  analogWriteResolution(PIN_LED_R, 8);
  analogWriteFrequency(PIN_LED_R, 5000);
  analogWriteResolution(PIN_LED_G, 8);
  analogWriteFrequency(PIN_LED_G, 5000);
  analogWriteResolution(PIN_LED_B, 8);
  analogWriteFrequency(PIN_LED_B, 5000);

  setRgb(ledR, ledG, ledB);

  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);

  // Koneksi WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Menghubungkan ke WiFi");
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 40) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    String ip = WiFi.localIP().toString();
    Serial.println("\nTerhubung! IP: " + ip);

    char buf1[17], buf2[17];
    snprintf(buf1, sizeof(buf1), "IP: %-13s", ip.c_str());
    lcdPrint("WiFi Terhubung!", buf1);
    delay(3000);

    // Tampilkan IP di LCD
    lcdPrint("Buka di browser:", buf1);
  } else {
    Serial.println("\nGagal terhubung WiFi!");
    lcdPrint("  WiFi GAGAL!   ", " Cek SSID/PASS  ");
  }

  // Setup routes
  server.on("/", handleRoot);
  server.on("/play", handlePlay);
  server.on("/timer", handleTimer);
  server.on("/stop", handleStop);
  server.on("/status", handleStatus);
  server.on("/setlcd", handleSetLCD);
  server.on("/setledrgb", handleSetLedRgb);
  server.on("/setrelay", handleSetRelay);
  server.begin();
  Serial.println("Web server aktif di port 80");
}

// =====================================================================
// === LOOP ============================================================
// =====================================================================
void loop() {
  server.handleClient();
  delay(2);
}
