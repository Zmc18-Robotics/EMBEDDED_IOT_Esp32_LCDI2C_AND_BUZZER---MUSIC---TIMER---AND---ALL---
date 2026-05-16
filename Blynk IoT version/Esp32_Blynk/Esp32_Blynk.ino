/*
 * =====================================================================
 * ESP32 Blynk IoT Music Player PREMIUM HYBRID VERSION
 * Music + Timer + RGB LED + Relay + Rich Web UI + Blynk IoT
 * =====================================================================
 */

// --- BLYNK CONFIG ---
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Esp32 Music Timer"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"

/*
 * =====================================================================
 * 📘 PANDUAN KONFIGURASI BLYNK DATASTREAM (Web Dashboard)
 * =====================================================================
 * Silakan buat Datastream di Blynk Console dengan detail berikut:
 *
 * [PIN]  [NAMA]           [TYPE]      [KETERANGAN]
 * V0     Song Selector    Integer     Pilih lagu (0-8)
 * V1     Stop Button      Integer     Berhenti (0/1)
 * V2     Timer Slider     Integer     Atur detik (1-3600)
 * V3     Start Timer      Integer     Mulai timer (0/1)
 * V4     LCD Line 1       String      Teks baris 1 LCD
 * V5     LCD Line 2       String      Teks baris 2 LCD
 * V6     Update LCD       Integer     Kirim teks ke LCD (0/1)
 * V7     Master LED       Integer     ON/OFF LED (0/1)
 * V8     LED Sync         Integer     Sinkron Musik (0/1)
 * V9     ZeRGBa           RGB         Warna manual (Tipe RGB)
 * V10    Relay Toggle     Integer     ON/OFF Relay (0/1)
 * V11-19 Hotkeys          Integer     Tombol lagu instan (0/1)
 * V20    Status           String      Info dari ESP32 ke Web
 * V21-23 RGB Toggles      Integer     Manual R, G, B (0/1)
 * V24-25 Timer +/-        Integer     Tambah/Kurang timer (0/1)
 * =====================================================================
 */

#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Wire.h>

// PIN DEFINITIONS
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

// --- KONFIGURASI WiFi ---
char auth[] = BLYNK_AUTH_TOKEN;
const char *WIFI_SSID = "YOUR_WIFI_NAME";
const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
WebServer server(80);

// STATE MESIN
volatile bool stopFlag = false;
volatile bool isPlaying = false;
volatile bool isTimer = false;
volatile int currentSong = -1;
volatile int timerSeconds = 15;

bool ledOn = true;
bool ledSync = true;
int ledR = 255, ledG = 0, ledB = 0;
bool relayState = false;
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
String inputL1 = "";
String inputL2 = "";

// =====================================================================
// === FREKUENSI NADA ==================================================
// ==================================================================// Oktaf 0
// Mayor
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
#define MIm 330
#define FAm 370
#define SOLm 415
#define LAm 466
#define SIm 494
// Oktaf 1 Minor
#define DOm1 554
#define REm1 622
#define MIm1 659
#define FAm1 740
#define SOLm1 831
#define LAm1 932
#define SIm1 988
// Oktaf 2 Minor
#define DOm2 1109
#define REm2 1245
#define MIm2 1319
#define FAm2 1480
#define SOLm2 1661
#define LAm2 1865
#define SIm2 1976
// Oktaf 3 Minor
#define DOm3 2217
#define REm3 2489
#define MIm3 2637
#define FAm3 2960
#define SOLm3 3322
#define LAm3 3729
#define SIm3 3951
// Nada Tetris & Khusus
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
// Happy Birthday
#define SOL_HB 784
#define LA_HB 880
#define SI_HB 988
#define DO2_HB 1047
#define RE2_HB 1175
#define MI2_HB 1319
#define FA2_HB 1397
#define SOL2_HB 1568
// Fur Elise
#define FE_E4 330
#define FE_DS4 311
#define FE_D4 294
#define FE_C4 262
#define FE_B3 247
#define FE_A3 220
#define FE_GS3 208
#define FE_E3 165
#define FE_C3 131

#define DIAM 0

// Tempo Helper
#define BPM_FUR 120
#define BEAT_FUR (60000 / BPM_FUR)
#define Q_FUR BEAT_FUR
#define H_FUR (BEAT_FUR * 2)
#define E_FUR (BEAT_FUR / 2)
#define S_FUR (BEAT_FUR / 4)

#define BPM_TET 144
#define BEAT_TET (60000 / BPM_TET)
#define Q_TET BEAT_TET
#define H_TET (BEAT_TET * 2)
#define E_TET (BEAT_TET / 2)
#define DQ_TET (BEAT_TET * 3 / 2)

// =====================================================================
// === CUSTOM CHARACTERS ===============================================
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
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
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
const char *nokiaTeks1[] = {
    "Nokia...",    "Nokia..",    "Nokia.",     "Nokia!",
    "Gran Vals",   "Gran Vals~", "~",          "Nokia!",
    "Francisco",   "Tarrega",    "1902",       "Nokia!",
    "** NOKIA **", "",           "Nokia...",   "Nokia..",
    "Nokia.",      "Nokia!",     "Calling...", "Calling..",
    "Calling.",    "INCOMING!",  "Ring ring~", "Ring ring!",
    "~Nokia~",     "NOKIA!!!",   "Gotta pick", "up that",
    "classic",     "ringtone~",  "",           ""};
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
                            "",
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
                                "Du du bi du bi",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                ""};
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
                                "La da",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                "",
                                ""};

// =====================================================================
// === LAGU 8: KISS ME AGAIN — REPAINT =================================
// =====================================================================
const int totalNadaKmA = 97;
const int melodiKmA[] = {
    FA2,  FA2, MI2,  FA2,  LA2, FA2,  DO2, LA1,  DO2,  MI2, FA2,  FA2, DO2,
    LAm1, LA1, DIAM, LA2,  FA2, DIAM, DO2, LA1,  DO2,  MI2, FA2,  FA2, DO2,
    LAm1, LA1, DIAM, LA2,  FA2, DIAM, DO2, LA1,  DO2,  MI2, FA2,  FA2, DO2,
    LAm1, LA1, DIAM, LA2,  FA2, LA2,  FA2, SOL2, SOL2, RE2, MI2,  LA2, FA2,
    DIAM, DO2, LA1,  DO2,  MI2, FA2,  FA2, DO2,  LAm1, LA1, DIAM, LA2, FA2,
    DIAM, DO2, LA1,  DO2,  MI2, FA2,  FA2, DO2,  LAm1, LA1, DIAM, LA2, FA2,
    DIAM, DO2, LA1,  DO2,  MI2, FA2,  FA2, DO2,  LAm1, LA1, DIAM, LA2, FA2,
    LA2,  FA2, SOL2, SOL2, RE2, MI2};
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
const int durasiKmA[] = {
    200, 200, 200, 200, 400, 400, 200, 200, 200, 200, 400, 400, 200, 200,
    200, 200, 400, 400, 100, 200, 200, 200, 200, 400, 400, 200, 200, 200,
    200, 400, 400, 100, 200, 200, 200, 200, 400, 400, 200, 200, 200, 200,
    400, 400, 400, 400, 300, 300, 200, 800, 400, 400, 100, 200, 200, 200,
    200, 400, 400, 200, 200, 200, 200, 400, 400, 100, 200, 200, 200, 200,
    400, 400, 200, 200, 200, 200, 400, 400, 100, 200, 200, 200, 200, 400,
    400, 200, 200, 200, 200, 400, 400, 400, 400, 300, 300, 200, 800};
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

// =====================================================================
// === LAGU 9: TETRIS THEME ============================================
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
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
// =====================================================================
// === HELPERS =========================================================
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
  if (!ledOn || !ledSync) return;
  
  if (freq <= 0) {
    setRgb(0, 0, 0);
    return;
  }
  if (freq < 300)
    setRgb(255, 0, 0);
  else if (freq < 400)
    setRgb(255, 127, 0);
  else if (freq < 500)
    setRgb(255, 255, 0);
  else if (freq < 700)
    setRgb(0, 255, 0);
  else if (freq < 1000)
    setRgb(0, 0, 255);
  else if (freq < 1500)
    setRgb(75, 0, 130);
  else
    setRgb(143, 0, 255);
}

void lcdPrint(const char *b1, const char *b2) {
  // Debug ke Serial Monitor
  Serial.print("LCD L1: ["); Serial.print(b1); Serial.println("]");
  Serial.print("LCD L2: ["); Serial.print(b2); Serial.println("]");

  char buf1[17] = "                "; // 16 spasi
  char buf2[17] = "                ";
  
  if (b1) {
    int len = strlen(b1);
    if (len > 16) len = 16;
    memcpy(buf1, b1, len);
    buf1[16] = '\0';
  }
  
  if (b2) {
    int len = strlen(b2);
    if (len > 16) len = 16;
    memcpy(buf2, b2, len);
    buf2[16] = '\0';
  }

  lcd.setCursor(0, 0);
  lcd.print(buf1);
  lcd.setCursor(0, 1);
  lcd.print(buf2);
}

void safeDelay(int ms) {
  unsigned long start = millis();
  while (millis() - start < (unsigned long)ms) {
    if (stopFlag)
      return;
    delay(10);
  }
}

void playNote(int freq, int dur, float ratio = 0.85) {
  if (stopFlag)
    return;

  if (freq <= 0) {
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
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
    if (offTime > 0)
      delay(offTime);
  }
}

// =====================================================================
// === BLYNK HANDLERS ==================================================
// =====================================================================
void startMusicTask(int mode, int value);

BLYNK_CONNECTED() {
  Blynk.virtualWrite(V7, ledOn);
  Blynk.virtualWrite(V8, ledSync);
  Blynk.virtualWrite(V10, relayState);
  Blynk.virtualWrite(V2, timerSeconds);
  Blynk.virtualWrite(V21, ledR > 0);
  Blynk.virtualWrite(V22, ledG > 0);
  Blynk.virtualWrite(V23, ledB > 0);
  // Ambil data teks terakhir dari aplikasi
  Blynk.syncVirtual(V4, V5);
}

// --- BLYNK WRITE HANDLERS ---
// V0: [Integer] Memilih lagu dari menu (0-8)
BLYNK_WRITE(V0) {
  int songId = param.asInt();
  if (songId >= 0 && songId <= 8)
    startMusicTask(0, songId);
}

// V1: [Integer] Tombol Stop untuk mematikan semua fungsi
BLYNK_WRITE(V1) {
  if (param.asInt() == 1) {
    stopFlag = true;
    noTone(BUZZER_PIN);
    setRgb(0, 0, 0);
    digitalWrite(PIN_RELAY, LOW);
    relayState = false;
    Blynk.virtualWrite(V10, 0);
  }
}

// V2: [Integer] Slider untuk mengatur durasi timer
BLYNK_WRITE(V2) { timerSeconds = param.asInt(); }

// V3: [Integer] Tombol Start untuk memulai hitung mundur
BLYNK_WRITE(V3) {
  if (param.asInt() == 1)
    startMusicTask(1, timerSeconds);
}

// V4: [String] Input teks untuk baris 1 LCD
BLYNK_WRITE(V4) { 
  inputL1 = param.asString(); 
  Serial.print("V4 Received: "); 
  Serial.println(inputL1);
}

// V5: [String] Input teks untuk baris 2 LCD
BLYNK_WRITE(V5) { 
  inputL2 = param.asString(); 
  Serial.print("V5 Received: "); 
  Serial.println(inputL2);
}

// V6: [Integer] Tombol untuk update teks ke LCD fisik
BLYNK_WRITE(V6) {
  if (param.asInt() == 1) {
    Serial.println("V6 Pressed: Updating LCD...");
    
    // Berhentikan semua aktivitas
    stopFlag = true;
    isPlaying = false;
    isTimer = false;
    
    noTone(BUZZER_PIN);
    setRgb(0, 0, 0);
    
    // Tampilkan pesan loading di LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Syncing Data...");
    
    // Paksa ambil data terbaru dari server Blynk
    Blynk.syncVirtual(V4, V5);
    
    // PENTING: Panggil Blynk.run() beberapa kali agar paket sync diproses
    for (int i = 0; i < 10; i++) {
      Blynk.run();
      delay(50);
    }
    
    Serial.print("Final L1: "); Serial.println(inputL1);
    Serial.print("Final L2: "); Serial.println(inputL2);
    
    lcd.clear();
    if (inputL1.length() == 0 && inputL2.length() == 0) {
      lcdPrint("Data Masih Kosong", "Cek Koneksi App");
    } else {
      lcdPrint(inputL1.c_str(), inputL2.c_str());
    }
    
    Blynk.virtualWrite(V20, "LCD Updated!"); 
  }
}
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
// V7: [Integer] Switch Master ON/OFF untuk lampu LED
BLYNK_WRITE(V7) {
  ledOn = param.asInt() == 1;
  if (!ledSync)
    setRgb(ledR, ledG, ledB);
  else if (!ledOn)
    setRgb(0, 0, 0);
}

// V8: [Integer] Switch Mode LED (Musik vs Manual)
BLYNK_WRITE(V8) { ledSync = param.asInt() == 1; }

// V9: [RGB] Color picker untuk warna manual
BLYNK_WRITE(V9) {
  ledR = param[0].asInt();
  ledG = param[1].asInt();
  ledB = param[2].asInt();
  if (!ledSync && ledOn)
    setRgb(ledR, ledG, ledB);
}

// V10: [Integer] Switch Manual untuk Relay
BLYNK_WRITE(V10) {
  relayState = param.asInt() == 1;
  digitalWrite(PIN_RELAY, relayState ? HIGH : LOW);
}

// V21-V23: [Integer] Slider manual untuk masing-masing warna R, G, B (Range
// 0-255)
BLYNK_WRITE(V21) {
  ledR = param.asInt();
  if (!ledSync && ledOn)
    setRgb(ledR, ledG, ledB);
}
BLYNK_WRITE(V22) {
  ledG = param.asInt();
  if (!ledSync && ledOn)
    setRgb(ledR, ledG, ledB);
}
BLYNK_WRITE(V23) {
  ledB = param.asInt();
  if (!ledSync && ledOn)
    setRgb(ledR, ledG, ledB);
}

// V24-V25: [Integer] Tombol Tambah/Kurang Timer (+10s / -10s)
BLYNK_WRITE(V24) {
  if (param.asInt() == 1) {
    timerSeconds += 10;
    if (timerSeconds > 3600)
      timerSeconds = 3600;
    Blynk.virtualWrite(V2, timerSeconds);
  }
}
BLYNK_WRITE(V25) {
  if (param.asInt() == 1) {
    timerSeconds -= 10;
    if (timerSeconds < 1)
      timerSeconds = 1;
    Blynk.virtualWrite(V2, timerSeconds);
  }
}

// Song Hotkeys (V11-V19): [Integer] Tombol instan per lagu
BLYNK_WRITE(V11) {
  if (param.asInt() == 1)
    startMusicTask(0, 0);
}
BLYNK_WRITE(V12) {
  if (param.asInt() == 1)
    startMusicTask(0, 1);
}
BLYNK_WRITE(V13) {
  if (param.asInt() == 1)
    startMusicTask(0, 2);
}
BLYNK_WRITE(V14) {
  if (param.asInt() == 1)
    startMusicTask(0, 3);
}
BLYNK_WRITE(V15) {
  if (param.asInt() == 1)
    startMusicTask(0, 4);
}
BLYNK_WRITE(V16) {
  if (param.asInt() == 1)
    startMusicTask(0, 5);
}
BLYNK_WRITE(V17) {
  if (param.asInt() == 1)
    startMusicTask(0, 6);
}
BLYNK_WRITE(V18) {
  if (param.asInt() == 1)
    startMusicTask(0, 7);
}
BLYNK_WRITE(V19) {
  if (param.asInt() == 1)
    startMusicTask(0, 8);
}

// =====================================================================
// === MUSIC ENGINE ====================================================
// =====================================================================
TaskHandle_t musicTaskHandle = NULL;
struct TaskParams {
  int mode;
  int value;
};
TaskParams taskParams;

void playSong(int songId) {
  stopFlag = false;
  isPlaying = true;
  isTimer = false;
  currentSong = songId;
  String names[] = {"Fur Elise",  "Happy Birthday", "Tsundere",
                    "Scale Demo", "Nokia",          "Super Mario",
                    "Bumblebee",  "Kiss Me Again",  "Tetris"};
  Blynk.virtualWrite(V20, "Playing: " + names[songId]);

  // -------- LAGU 1: FUR ELISE --------
  if (songId == 0) {
    lcdPrint("Fur Elise", "Beethoven 1810");
    safeDelay(1800);
    for (int i = 0; i < furEliseTotal && !stopFlag; i++) {
      lcdPrint(furEliseTeks1[i], furEliseTeks2[i]);
      playNote(furEliseMelodi[i], furEliseDurasi[i]);
    }
  }
  // -------- LAGU 2: HAPPY BIRTHDAY --------
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
    safeDelay(2000);
    for (int i = 0; i < happyBirthTotal && !stopFlag; i++) {
      lcd.setCursor(0, 0);
      char buf[17];
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
  }
  // -------- LAGU 3: ITS NOT LIKE I LIKE YOU --------
  else if (songId == 2) {
    lcdPrint("Music Player", "Its Not Like...");
    safeDelay(1500);
    for (int i = 0; i < itsNotTotal && !stopFlag; i++) {
      lcdPrint(itsNotTeks1[i], itsNotTeks2[i]);
      playNote(itsNotMelodi[i], itsNotDurasi[i]);
      delay(50);
    }
  }
  // -------- LAGU 4: MAYOR MINOR SCALE --------
  else if (songId == 3) {
    lcdPrint("Mayor Minor", "Scale Demo");
    safeDelay(1500);
    for (int i = 0; i < mayorMinorTotal && !stopFlag; i++) {
      lcdPrint(mayorMinorTeks1[i], mayorMinorTeks2[i]);
      playNote(mayorMinorMelodi[i], mayorMinorDurasi[i]);
      delay(50);
    }
  }
  // -------- LAGU 5: NOKIA RINGTONE --------
  else if (songId == 4) {
    lcdPrint("Nokia Ringtone", "Gran Vals 1902");
    safeDelay(1500);
    for (int i = 0; i < nokiaTotal && !stopFlag; i++) {
      lcdPrint(nokiaTeks1[i], nokiaTeks2[i]);
      playNote(nokiaMelodi[i], nokiaDurasi[i]);
      delay(50);
    }
  }
  // -------- LAGU 6: SUPER MARIO --------
  else if (songId == 5) {
    lcdPrint("Super Mario", "Nintendo 1985");
    safeDelay(1800);
    for (int i = 0; i < marioTotal && !stopFlag; i++) {
      lcdPrint(marioTeks1[i], marioTeks2[i]);
      playNote(marioMelodi[i], marioDurasi[i]);
      delay(40);
    }
  }
  // -------- LAGU 7: SWEET LITTLE BUMBLEBEE --------
  else if (songId == 6) {
    lcdPrint("Sweet Bumblebee", "Siap...");
    delay(1500);
    for (int i = 0; i < bumblebeeTotal && !stopFlag; i++) {
      lcdPrint(bumblebeeTeks1[i], bumblebeeTeks2[i]);
      playNote(bumblebeeMelodi[i], bumblebeeDurasi[i]);
      delay(50);
    }
  }
  // -------- LAGU 8: KISS ME AGAIN --------
  else if (songId == 7) {
    lcdPrint("Kiss Me Again", "Siap...");
    delay(1500);
    for (int i = 0; i < totalNadaKmA && !stopFlag; i++) {
      lcdPrint(teks1KmA[i], teks2KmA[i]);
      playNote(melodiKmA[i], durasiKmA[i], 1.0);
      delay(50);
    }
  }
  // -------- LAGU 9: TETRIS --------
  else if (songId == 8) {
    lcdPrint("Tetris Theme", "Korobeiniki");
    delay(1500);
    for (int i = 0; i < tetrisTotal && !stopFlag; i++) {
      lcdPrint(tetrisTeks1[i], tetrisTeks2[i]);
      playNote(tetrisMelodi[i], tetrisDurasi[i]);
      delay(40);
    }
  }

  noTone(BUZZER_PIN);
  if (ledSync)
    setRgb(0, 0, 0);
  isPlaying = false;
  currentSong = -1;
  Blynk.virtualWrite(V20, "System Ready");
}
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
int beepIntervalMs[] = {900, 850, 800, 720, 640, 560, 480, 400,
                        330, 270, 220, 180, 150, 120, 100};

void runTimer(int totalSec) {
  stopFlag = false;
  isTimer = true;
  Blynk.virtualWrite(V20, "Timer Running");

  for (int detik = totalSec; detik >= 1 && !stopFlag; detik--) {
    int m = detik / 60, s = detik % 60;
    char b1[17], b2[17];
    snprintf(b1, sizeof(b1), "  Hitung Mundur ");
    snprintf(b2, sizeof(b2), "     %02d:%02d      ", m, s);
    lcdPrint(b1, b2);

    // 15 Detik Terakhir: Beep makin cepat
    if (detik <= 15) {
      int idx = 15 - detik;
      int interval = beepIntervalMs[idx];
      int freq = 880 + (idx * 80);
      if (freq > 2000) freq = 2000;

      unsigned long startSec = millis();
      while (millis() - startSec < 1000 && !stopFlag) {
        // Nyalakan Buzzer & LED
        tone(BUZZER_PIN, freq);
        if (ledOn && ledSync) setRgb(255, 255, 255);
        delay(50);
        
        // Matikan Buzzer & LED
        noTone(BUZZER_PIN);
        if (ledOn && ledSync) setRgb(0, 0, 0);
        
        int wait = interval - 50;
        if (wait < 10) wait = 10;
        
        unsigned long startWait = millis();
        while(millis() - startWait < (unsigned long)wait && !stopFlag) {
          delay(5);
        }
        if (millis() - startSec >= 1000) break;
      }
    } 
    // Di atas 15 detik: Beep sekali tiap detik
    else {
      if (ledOn && ledSync) setRgb(255, 255, 255);
      tone(BUZZER_PIN, 1000);
      delay(100);
      noTone(BUZZER_PIN);
      if (ledOn && ledSync) setRgb(0, 0, 0);
      
      safeDelay(900);
    }
  }

  // ALARM SELESAI
  if (!stopFlag) {
    lcdPrint("   TIME OUT!!   ", "  Relay Active  ");
    Blynk.virtualWrite(V20, "TIME OUT!!");
    
    for (int i = 0; i < 5 && !stopFlag; i++) {
      tone(BUZZER_PIN, 1200);
      if (ledOn && ledSync) setRgb(255, 0, 0); 
      delay(500);
      noTone(BUZZER_PIN);
      if (ledOn && ledSync) setRgb(0, 0, 0);
      delay(500);
    }

    if (!stopFlag) {
      digitalWrite(PIN_RELAY, HIGH);
      relayState = true;
      Blynk.virtualWrite(V10, 1);
      safeDelay(3000);
      digitalWrite(PIN_RELAY, LOW);
      relayState = false;
      Blynk.virtualWrite(V10, 0);
    }
  }

  noTone(BUZZER_PIN);
  setRgb(0, 0, 0);
  isTimer = false;
  Blynk.virtualWrite(V20, "System Ready");
}

void musicTask(void *pvParams) {
  TaskParams *p = (TaskParams *)pvParams;
  if (p->mode == 0)
    playSong(p->value);
  else
    runTimer(p->value);
  musicTaskHandle = NULL;
  vTaskDelete(NULL);
}

void startMusicTask(int mode, int value) {
  if (musicTaskHandle != NULL) {
    stopFlag = true;
    delay(300);
    vTaskDelete(musicTaskHandle);
    musicTaskHandle = NULL;
    noTone(BUZZER_PIN);
    setRgb(0, 0, 0);
    isPlaying = false;
    isTimer = false;
  }
  taskParams.mode = mode;
  taskParams.value = value;
  stopFlag = false;
  xTaskCreatePinnedToCore(musicTask, "music", 8192, &taskParams, 1,
                          &musicTaskHandle, 1);
}

// =====================================================================
// === WEB UI HANDLERS =================================================
// =====================================================================
void handleRoot() {
  server.send(200, "text/html",
              "<html><body style='font-family:sans-serif; text-align:center; "
              "background:#121212; color:white;'><h1>ESP32 Music "
              "Premium</h1><p>Blynk IoT is active.</p><p>IP: " +
                  WiFi.localIP().toString() +
                  "</p><button onclick=\"fetch('/stop')\" style='padding:20px; "
                  "background:red; color:white; border:none; "
                  "border-radius:10px;'>STOP ALL</button></body></html>");
}
void handleStop() {
  stopFlag = true;
  delay(100);
  noTone(BUZZER_PIN);
  isPlaying = false;
  isTimer = false;
  lcdPrint("System Stopped", "Web Command");
  server.send(200, "text/plain", "Stopped");
}

// =====================================================================
// === SETUP & LOOP ====================================================
// =====================================================================
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();
  Wire.setClock(400000); // Set I2C ke High Speed (400kHz) agar lirik sinkron
  lcdPrint("Initializing...", "Premium Hybrid");
//by Zmc18-Robotics ~ @mc.zminecrafter_18 ~ Zmc18_Roboticz
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);

  // FIX LED REDUP (ESP32 Core 3.x): Set resolusi 8-bit
  analogWriteResolution(PIN_LED_R, 8);
  analogWriteResolution(PIN_LED_G, 8);
  analogWriteResolution(PIN_LED_B, 8);
  // Gunakan frekuensi default 1000Hz untuk menghindari konflik timer dengan tone()

  Blynk.begin(auth, WIFI_SSID, WIFI_PASSWORD);
  server.on("/", handleRoot);
  server.on("/stop", handleStop);
  server.begin();
  lcdPrint("System Ready!", WiFi.localIP().toString().c_str());
}

void loop() {
  Blynk.run();
  server.handleClient();
  delay(2);
}
