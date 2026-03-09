# Pomodoro Arduino Timer
Physical Hardware Timer using an 8×8 NeoMatrix, button, and potentiometer. The timer cycles through 80 pixel-art animation frames on an LED matrix to visualize countdown progress.

The arduino code is located in this repo.

---

### Device Specs:
- Arduino ELEGOO UNO R3
- Adafruit NeoMatrix (8×8 WS2812 NeoPixel)
- B10K Potentiometer
- Two-legged momentary push button
- 3.3 Ω Resistor
- Standard Wires

### Required Libraries:
- [FastLED](https://github.com/FastLED/FastLED)
- [FastLED_NeoMatrix](https://github.com/marcmerlin/FastLED_NeoMatrix)
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)

---

### How It Works:

The sketch runs a three-state machine: **SETTING → RUNNING → DONE**.

#### Startup
On boot, a pixel-art tomato sprite is displayed for 2 seconds as a splash screen, then the timer transitions into SETTING mode.

#### SETTING Mode
- The potentiometer (pin A0) is read to select a session duration between 1 and 60 minutes.
- The matrix shows a green fill bar: more lit pixels = more time selected. At minimum, 1 pixel is lit; at maximum, all 64 pixels are lit.
- Press the button (pin D3) to lock in the time and start the countdown.

#### RUNNING Mode
- The total session time is divided evenly across 80 animation frames. For example, a 40-minute session advances one frame every 30 seconds.
- Each frame is a `uint64_t` bitmap stored in flash (PROGMEM) and rendered pixel-by-pixel onto the 8×8 matrix using a configurable RGB565 foreground color.
- The frames depict an animated sequence that progresses as the session counts down.
- Press the button at any time to cancel and return to SETTING mode.

#### DONE Mode
- When all 80 frames have elapsed, a green checkmark image is displayed.
- Press the button to reset back to SETTING mode for the next session.

---

### Pin Configuration:
| Pin | Role |
|-----|------|
| D8  | NeoMatrix data-in |
| D3  | Momentary button (active LOW, internal pull-up) |
| A0  | Potentiometer wiper |

---

### Customizing Pixel Art:

The 80 animation frames and a separate "done" image are all user-replaceable:

1. Open the [LED Editor](https://xantorohara.github.io/led-editor/) and draw an 8×8 image.
2. Copy the `uint64_t` hex value shown by the editor.
3. Paste it into the `frames[]` array in the sketch (stored in PROGMEM).
4. Set a color for that frame in `frameColors[]` using the provided `COL_*` RGB565 constants (`COL_WHITE`, `COL_RED`, `COL_GREEN`, `COL_BLUE`, `COL_YELLOW`, `COL_ORANGE`, `COL_CYAN`, `COL_MAGENTA`) or any custom 16-bit RGB565 value.

**Bit layout:** MSB byte = row 0 (top), bit 7 = col 0 (left).

---

### Serial Output:
The sketch logs state transitions and frame progress over Serial at 9600 baud, useful for debugging without a display.

---

### Demo Video:
(Coming Soon)
