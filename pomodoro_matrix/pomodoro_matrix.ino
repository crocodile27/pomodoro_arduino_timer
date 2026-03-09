// ================================================================
//  POMODORO MATRIX TIMER  –  NeoPixel / FastLED_NeoMatrix edition
//  Hardware : Arduino Uno/Nano
//             Adafruit NeoMatrix 8×8 (WS2812 / NeoPixel)
//             Potentiometer on A0
//             Momentary push-button on D3 (active LOW, uses pullup)
//
//  Libraries required (install via Library Manager):
//    • FastLED
//    • FastLED_NeoMatrix
//    • Adafruit GFX Library
//
//  HOW IT WORKS:
//    SETTING  – Rotate pot to pick 5–60 min.
//               Matrix shows a green fill bar (more = more time).
//               Press button to lock time in and start.
//    RUNNING  – Steps through 80 pixel-art frames, one per equal
//               slice of the total time you set.
//               e.g. 40 min ÷ 80 frames = 30 s per frame.
//               Press button at any time to cancel back to SETTING.
//    DONE     – Shows a "done" image. Press button to reset.
//
//  ADDING YOUR PIXEL ART:
//    1. Open https://xantorohara.github.io/led-editor/
//    2. Draw your 8×8 image, copy the uint64_t hex value shown.
//    3. Paste it into frames[] below.
//    4. Set a colour for that frame in frameColors[] using
//       matrix->Color(R, G, B) — handy COL_* constants are defined
//       near the top of the sketch.
//    Bit layout: MSB byte = row 0 (top), bit 7 = col 0 (left).
// ================================================================

#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include <Adafruit_GFX.h>

// ----------------------------------------------------------------
//  PIN CONFIGURATION
// ----------------------------------------------------------------
#define MATRIX_PIN   8    // NeoMatrix data-in pin
#define BUTTON_PIN   3    // Digital – connect pin to GND via button
#define POT_PIN      A0   // Analog  – wiper of potentiometer

// ----------------------------------------------------------------
//  MATRIX SETUP
//  Adjust the layout flags to match how your matrix is wired.
//  Common alternatives for the 4th constructor argument:
//    NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT  + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG
//    NEO_MATRIX_TOP    + NEO_MATRIX_LEFT  + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE
// ----------------------------------------------------------------
#define MATRIX_W  8
#define MATRIX_H  8
#define NUMPIXELS (MATRIX_W * MATRIX_H)

CRGB leds[NUMPIXELS];

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(
  leds, MATRIX_W, MATRIX_H,
  NEO_MATRIX_TOP    + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS   + NEO_MATRIX_PROGRESSIVE + NEO_MATRIX_ZIGZAG
);

// ----------------------------------------------------------------
//  HANDY COLOUR CONSTANTS  (RGB565)
//  Use these directly in frameColors[] below.
// ----------------------------------------------------------------
#define COL_WHITE   0xFFFF
#define COL_RED     0xF800
#define COL_GREEN   0x07E0
#define COL_BLUE    0x001F
#define COL_YELLOW  0xFFE0
#define COL_ORANGE  0xFD00
#define COL_CYAN    0x07FF
#define COL_MAGENTA 0xF81F

// ----------------------------------------------------------------
//  TIMER PARAMETERS
// ----------------------------------------------------------------
#define NUM_IMAGES    80
#define MIN_MINUTES    1
#define MAX_MINUTES   60

// ----------------------------------------------------------------
//  PIXEL-ART FRAMES  (stored in flash via PROGMEM)
//
//  Replace each 0x0000000000000000 with your own uint64_t value
//  from the LED editor linked above.
// ----------------------------------------------------------------
const uint64_t frames[] PROGMEM = {
  0x00000000183c7eff,
  0x00000000183c7ef7,
  0x00000008183c7ef7,
  0x00000800183c7ef7,
  0x00080000183c7ef7,
  0x08000000183c7ef7,
  0x08000000183c7ee7,
  0x08000008183c7ee7,
  0x08000800183c7ee7,
  0x08080000183c7ee7,
  0x18000000183c7ee7,
  0x18000000183c7ee3,
  0x18000008183c7ee3,
  0x18000800183c7ee3,
  0x18080000183c7ee3,
  0x1c000000183c7ee3,
  0x1c000000183c7ec3,
  0x1c000010183c7ec3,
  0x1c001000183c7ec3,
  0x1c100000183c7ec3,
  0x1c100000183c7ec3,
  0x1c100000183c7ec1,
  0x1c100008183c7ec1,
  0x1c100800183c7ec1,
  0x1c180000183c7ec1,
  0x3c100000183c7ec1,
  0x3c100000183c7e81,
  0x3c100010183c7e81,
  0x3c101000183c7e81,
  0x3c180000183c7e81,
  0x3c180000183c7e80,
  0x3c180008183c7e80,
  0x3c180800183c7e80,
  0x3c1c0000183c7e80,
  0x3e180000183c7e80,
  0x3e180000183c7e00,
  0x3e180010183c7e00,
  0x3e181000183c7e00,
  0x3e380000183c7e00,
  0x7e180000183c7e00,
  0x7e180000183c7600,
  0x7e180008183c7600,
  0x7e180800183c7600,
  0x7e1c0000183c7600,
  0x7e1c0000183c6600,
  0x7e1c0010183c6600,
  0x7e1c1000183c6600,
  0x7e3c0000183c6600,
  0x7e3c0000183c6200,
  0x7e3c0008183c6200,
  0x7e3c0800183c6200,
  0x7e3c0800183c4200,
  0x7e3c0810183c4200,
  0x7e3c1800183c4200,
  0x7e3c1800183c0200,
  0x7e3c1810183c0200,
  0x7e3c3800183c0200,
  0x7e7c1800183c0200,
  0xfe3c1800183c0200,
  0xfe3c1800183c0000,
  0xfe3c1808183c0000,
  0xfe3c1c00183c0000,
  0xfe3e1800183c0000,
  0xff3c1800183c0000,
  0xff3c180018340000,
  0xff3c180818340000,
  0xff3c1c0018340000,
  0xff3e180018340000,
  0xff3e180018240000,
  0xff3e181018240000,
  0xff3e380018240000,
  0xff7e180018240000,
  0xff7e180018040000,
  0xff7e181018040000,
  0xff7e380018040000,
  0xff7e380018000000,
  0xff7e380818000000,
  0xff7e3c0018000000,
  0xff7e3c0010000000,
  0xff7e3c0810000000,
  0xff7e3c1800000000
};

#define NUM_IMAGES (sizeof(frames) / sizeof(frames[0]))
// ----------------------------------------------------------------
//  PER-FRAME COLOURS  (RGB565, one per frame)
//  Use the COL_* constants above, or any 16-bit RGB565 value.
// ----------------------------------------------------------------
const uint16_t frameColors[NUM_IMAGES] PROGMEM = {
  COL_WHITE, // frame  0  ← change to any COL_* constant
  COL_WHITE, // frame  1
  COL_WHITE, // frame  2
  COL_WHITE, // frame  3
  COL_WHITE, // frame  4
  COL_WHITE, // frame  5
  COL_WHITE, // frame  6
  COL_WHITE, // frame  7
  COL_WHITE, // frame  8
  COL_WHITE, // frame  9
  COL_WHITE, // frame 10
  COL_WHITE, // frame 11
  COL_WHITE, // frame 12
  COL_WHITE, // frame 13
  COL_WHITE, // frame 14
  COL_WHITE, // frame 15
  COL_WHITE, // frame 16
  COL_WHITE, // frame 17
  COL_WHITE, // frame 18
  COL_WHITE, // frame 19
  COL_WHITE, // frame 20
  COL_WHITE, // frame 21
  COL_WHITE, // frame 22
  COL_WHITE, // frame 23
  COL_WHITE, // frame 24
  COL_WHITE, // frame 25
  COL_WHITE, // frame 26
  COL_WHITE, // frame 27
  COL_WHITE, // frame 28
  COL_WHITE, // frame 29
  COL_WHITE, // frame 30
  COL_WHITE, // frame 31
  COL_WHITE, // frame 32
  COL_WHITE, // frame 33
  COL_WHITE, // frame 34
  COL_WHITE, // frame 35
  COL_WHITE, // frame 36
  COL_WHITE, // frame 37
  COL_WHITE, // frame 38
  COL_WHITE, // frame 39
  COL_WHITE, // frame 40
  COL_WHITE, // frame 41
  COL_WHITE, // frame 42
  COL_WHITE, // frame 43
  COL_WHITE, // frame 44
  COL_WHITE, // frame 45
  COL_WHITE, // frame 46
  COL_WHITE, // frame 47
  COL_WHITE, // frame 48
  COL_WHITE, // frame 49
  COL_WHITE, // frame 50
  COL_WHITE, // frame 51
  COL_WHITE, // frame 52
  COL_WHITE, // frame 53
  COL_WHITE, // frame 54
  COL_WHITE, // frame 55
  COL_WHITE, // frame 56
  COL_WHITE, // frame 57
  COL_WHITE, // frame 58
  COL_WHITE, // frame 59
  COL_WHITE, // frame 60
  COL_WHITE, // frame 61
  COL_WHITE, // frame 62
  COL_WHITE, // frame 63
  COL_WHITE, // frame 64
  COL_WHITE, // frame 65
  COL_WHITE, // frame 66
  COL_WHITE, // frame 67
  COL_WHITE, // frame 68
  COL_WHITE, // frame 69
  COL_WHITE, // frame 70
  COL_WHITE, // frame 71
  COL_WHITE, // frame 72
  COL_WHITE, // frame 73
  COL_WHITE, // frame 74
  COL_WHITE, // frame 75
  COL_WHITE, // frame 76
  COL_WHITE, // frame 77
  COL_WHITE, // frame 78
  COL_WHITE, // frame 79
};

// Shown when session finishes — simple checkmark, green.
// Replace the uint64_t with your own "done" art if preferred.
const uint64_t doneImage PROGMEM = 0x3048840201000000ULL;

// ----------------------------------------------------------------
//  STATE MACHINE
// ----------------------------------------------------------------
enum State { SETTING, RUNNING, STATE_DONE }; // NOTE: "DONE" is reserved by FastLED on AVR
State currentState = SETTING;

int           selectedMinutes = 25;
int           currentFrame    = 0;
unsigned long frameStartTime  = 0;
unsigned long frameDuration   = 0; // ms per frame

// ----------------------------------------------------------------
//  BUTTON  –  debounced, returns true exactly once per press
// ----------------------------------------------------------------
#define DEBOUNCE_MS 50

bool          rawButtonLast = HIGH;
bool          stableButton  = HIGH;
unsigned long debounceTimer = 0;

bool buttonJustPressed() {
  bool reading = digitalRead(BUTTON_PIN);
  if (reading != rawButtonLast) {
    debounceTimer = millis();
    rawButtonLast = reading;
  }
  if ((millis() - debounceTimer) > DEBOUNCE_MS) {
    if (reading != stableButton) {
      stableButton = reading;
      if (reading == LOW) return true;
    }
  }
  return false;
}

// ----------------------------------------------------------------
//  PROGMEM HELPERS
// ----------------------------------------------------------------
uint64_t getFrame(int i) {
  uint64_t v;
  memcpy_P(&v, &frames[i], sizeof(uint64_t));
  return v;
}

uint16_t getColor(int i) {
  uint16_t v;
  memcpy_P(&v, &frameColors[i], sizeof(uint16_t));
  return v;
}

uint64_t getDoneImage() {
  uint64_t v;
  memcpy_P(&v, &doneImage, sizeof(uint64_t));
  return v;
}

// ----------------------------------------------------------------
//  DRAWING HELPERS
// ----------------------------------------------------------------
// ----------------------------------------------------------------
//  TOMATO STARTUP SPRITE
//  0 = off, 1 = green, 2 = red
// ----------------------------------------------------------------
const uint8_t tomatoSprite[64] PROGMEM = {

  0,0,2,2,2,2,0,0,
  0,2,2,2,2,2,2,0,
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,
  2,2,2,2,2,2,2,2,
  1,2,2,1,2,1,2,1,
  1,1,1,1,1,1,1,1,
  0,1,1,1,1,1,1,0
};

// ----------------------------------------------------------------
//  DRAW TOMATO
// ----------------------------------------------------------------
void drawTomato() {

  matrix->fillScreen(0);

  for(int y=0;y<8;y++){
    for(int x=0;x<8;x++){

      uint8_t p = pgm_read_byte(&tomatoSprite[y*8+x]);

      if(p==1)
        matrix->drawPixel(x,y,matrix->Color(0,255,0));

      if(p==2)
        matrix->drawPixel(x,y,matrix->Color(255,0,0));
    }
  }

  matrix->show();
}
// Render a uint64_t bitmap with a given RGB565 foreground colour.
void drawImage(uint64_t img, uint16_t color) {
  matrix->fillScreen(0);
  for (int row = 0; row < 8; row++) {
    uint8_t rowBits = (uint8_t)(img >> (56 - row * 8));
    for (int col = 0; col < 8; col++) {
      if (rowBits & (0x80 >> col)) {
        matrix->drawPixel(col, row, color);
      }
    }
  }
  matrix->show();
}

// Green fill bar for SETTING mode.
// 5 min = 1 lit pixel, 60 min = all 64 pixels lit.
void drawSettingBar(int minutes) {
  int litPixels = map(minutes, MIN_MINUTES, MAX_MINUTES, 1, 64);
  matrix->fillScreen(0);
  uint16_t barColor = matrix->Color(0, 200, 50);
  for (int i = 0; i < litPixels; i++) {
    matrix->drawPixel(i % 8, i / 8, barColor);
  }
  matrix->show();
}

// ----------------------------------------------------------------
//  SETUP
// ----------------------------------------------------------------
void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);

  FastLED.addLeds<NEOPIXEL, MATRIX_PIN>(leds, NUMPIXELS);
  matrix->begin();
  matrix->setBrightness(10);

  // show tomato splash
  drawTomato();
  delay(2000);

  // then show timer setup screen
  drawSettingBar(selectedMinutes);

  Serial.println(F("=== Pomodoro NeoMatrix Timer ==="));
  Serial.println(F("Rotate pot to set time (5-60 min), then press button."));
}

// ----------------------------------------------------------------
//  MAIN LOOP
// ----------------------------------------------------------------
void loop() {
  switch (currentState) {

    // ------------------------------------------------------------
    case SETTING:
    // ------------------------------------------------------------
    {
      int newMinutes = map(analogRead(POT_PIN), 0, 1023,
                           MIN_MINUTES, MAX_MINUTES);
      if (newMinutes != selectedMinutes) {
        selectedMinutes = newMinutes;
        drawSettingBar(selectedMinutes);
        Serial.print(F("Selected: "));
        Serial.print(selectedMinutes);
        Serial.println(F(" min"));
      }

      if (buttonJustPressed()) {
        frameDuration  = ((unsigned long)selectedMinutes * 60UL * 1000UL)
                         / NUM_IMAGES;
        currentFrame   = 0;
        frameStartTime = millis();
        currentState   = RUNNING;
        drawImage(getFrame(0), getColor(0));

        Serial.print(F("Started! "));
        Serial.print(selectedMinutes);
        Serial.print(F(" min | "));
        Serial.print(frameDuration / 1000UL);
        Serial.println(F("s per frame"));
      }
      break;
    }

    // ------------------------------------------------------------
    case RUNNING:
    // ------------------------------------------------------------
    {
      if (millis() - frameStartTime >= frameDuration) {
        currentFrame++;
        if (currentFrame >= NUM_IMAGES) {
          currentState = STATE_DONE;
          drawImage(getDoneImage(), COL_GREEN);
          Serial.println(F("Session complete! Press button to reset."));
        } else {
          frameStartTime = millis();
          drawImage(getFrame(currentFrame), getColor(currentFrame));
          Serial.print(F("Frame "));
          Serial.print(currentFrame);
          Serial.print('/');
          Serial.println(NUM_IMAGES - 1);
        }
      }

      if (buttonJustPressed()) {
        currentState   = SETTING;
        selectedMinutes = map(analogRead(POT_PIN), 0, 1023,
                              MIN_MINUTES, MAX_MINUTES);
        drawSettingBar(selectedMinutes);
        Serial.println(F("Cancelled. Back to SETTING."));
      }
      break;
    }

    // ------------------------------------------------------------
    case STATE_DONE:
    // ------------------------------------------------------------
    {
      if (buttonJustPressed()) {
        currentState   = SETTING;
        selectedMinutes = map(analogRead(POT_PIN), 0, 1023,
                              MIN_MINUTES, MAX_MINUTES);
        drawSettingBar(selectedMinutes);
        Serial.println(F("Ready for next session."));
      }
      break;
    }
  }
}
