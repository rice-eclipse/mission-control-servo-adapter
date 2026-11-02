// Mission Control Servo Adapter
//
// Turns two switched DC inputs (0 V, or 12 V / 24 V) into two servo positions:
//   input off (0 V)    -> position 1 (off / safe)
//   input on (12-24 V) -> position 2
//
// INPUT_1 (J4) drives SERVO_1 (J1). INPUT_2 (J5) drives SERVO_2 (J2).
// A disconnected input reads 0 V, so its servo goes to position 1.
//
// LEDs:
//   LED1  on while servo 1 is in position 2
//   LED2  on while servo 2 is in position 2
//   LED3  status: slow blink = running, fast blink = an input is over 30 V

#include <Servo.h>

// ----- Servo end points (microseconds). Tune these to each linkage. -----
int servo1Pos1 = 1000;  // Servo 1 off / safe
int servo1Pos2 = 2000;  // Servo 1 on
int servo2Pos1 = 1000;  // Servo 2 off / safe
int servo2Pos2 = 2000;  // Servo 2 on

// ----- Input thresholds -----
// The 56k/10k divider and 12-bit ADC give about 124 counts per input volt:
//   0 V = 0, 4 V = 496, 8 V = 993, 12 V = 1489, 24 V = 2978, 30 V = 3723
const int ON_LEVEL = 993;          // Above 8 V: input is on
const int OFF_LEVEL = 496;         // Below 4 V: input is off
const int OVER_RANGE_LEVEL = 3723; // Above 30 V: wiring fault

const unsigned long DEBOUNCE_MS = 20;  // Input must hold this long to move a servo

// ----- Pins -----
const int LED1_PIN = PIN_PA3;
const int LED2_PIN = PIN_PA4;
const int STATUS_LED_PIN = PIN_PA5;
const int INPUT1_PIN = PIN_PD1;
const int INPUT2_PIN = PIN_PD2;
const int SERVO1_PIN = PIN_PA1;
const int SERVO2_PIN = PIN_PA2;

Servo servo1;
Servo servo2;

// Per-input state
bool input1On = false;
bool input2On = false;
bool servo1InPos2 = false;
bool servo2InPos2 = false;
unsigned long input1ChangedAt = 0;
unsigned long input2ChangedAt = 0;

unsigned long lastFaultAt = 0;
bool faultSeen = false;

// Reads an input and applies hysteresis: once on, it stays on until it
// drops below OFF_LEVEL.
bool readInput(int pin, bool wasOn) {
  int level = analogRead(pin);

  if (level > OVER_RANGE_LEVEL) {
    faultSeen = true;
    lastFaultAt = millis();
  }

  if (wasOn) {
    return level > OFF_LEVEL;
  } else {
    return level > ON_LEVEL;
  }
}

void setup() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);

  analogReference(VDD);
  analogReadResolution(12);

  // Start both servos in position 1. Set the position before attach() so
  // the very first pulse is position 1, not the library's default center.
  servo1.writeMicroseconds(servo1Pos1);
  servo2.writeMicroseconds(servo2Pos1);
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo1.writeMicroseconds(servo1Pos1);
  servo2.writeMicroseconds(servo2Pos1);

  // Watchdog: resets the board if loop() stops running for about 0.25 s.
  _PROTECTED_WRITE(WDT.CTRLA, WDT_PERIOD_256CLK_gc);
}

void loop() {
  unsigned long now = millis();

  // ----- Input 1 -> Servo 1 -----
  bool reading1 = readInput(INPUT1_PIN, input1On);
  if (reading1 != input1On) {
    input1On = reading1;
    input1ChangedAt = now;
  }
  if (input1On != servo1InPos2 && now - input1ChangedAt >= DEBOUNCE_MS) {
    servo1InPos2 = input1On;
    servo1.writeMicroseconds(servo1InPos2 ? servo1Pos2 : servo1Pos1);
    digitalWrite(LED1_PIN, servo1InPos2);
  }

  // ----- Input 2 -> Servo 2 -----
  bool reading2 = readInput(INPUT2_PIN, input2On);
  if (reading2 != input2On) {
    input2On = reading2;
    input2ChangedAt = now;
  }
  if (input2On != servo2InPos2 && now - input2ChangedAt >= DEBOUNCE_MS) {
    servo2InPos2 = input2On;
    servo2.writeMicroseconds(servo2InPos2 ? servo2Pos2 : servo2Pos1);
    digitalWrite(LED2_PIN, servo2InPos2);
  }

  // ----- Status LED -----
  // Fast blink for 1 s after any over-range reading, otherwise slow blink.
  if (faultSeen && now - lastFaultAt < 1000) {
    digitalWrite(STATUS_LED_PIN, (now / 100) % 2);
  } else {
    digitalWrite(STATUS_LED_PIN, (now / 500) % 2);
  }

  __asm__ __volatile__("wdr");  // Feed the watchdog
  delay(1);
}
