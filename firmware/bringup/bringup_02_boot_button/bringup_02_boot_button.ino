// Bring-up 02: BOOT button
//
// Checks SW1 and the R14 pull-up on PC3. Skip this test if SW1 isn't fitted.
//
// Expected:
//   - LED1 blinks slowly (1 Hz) as a heartbeat.
//   - LED2 is on while SW1 is held down.
//   - LED3 toggles once per press, which shows the press is debounced.
//     If LED3 sometimes toggles twice, increase DEBOUNCE_MS.

const uint8_t LED1 = PIN_PA3;
const uint8_t LED2 = PIN_PA4;
const uint8_t LED3 = PIN_PA5;
const uint8_t BOOT = PIN_PC3;  // Active low: R14 pulls up, SW1 pulls to GND

const unsigned long DEBOUNCE_MS = 30;

bool stablePressed = false;
bool lastReading = false;
unsigned long lastChangeMs = 0;
bool led3State = false;

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  // Internal pull-up as well, so the test still works if R14 is missing.
  pinMode(BOOT, INPUT_PULLUP);
}

void loop() {
  unsigned long now = millis();

  digitalWrite(LED1, (now / 500) % 2);

  bool reading = digitalRead(BOOT) == LOW;
  if (reading != lastReading) {
    lastReading = reading;
    lastChangeMs = now;
  }

  if (now - lastChangeMs >= DEBOUNCE_MS && reading != stablePressed) {
    stablePressed = reading;
    if (stablePressed) {
      led3State = !led3State;
      digitalWrite(LED3, led3State);
    }
  }

  digitalWrite(LED2, stablePressed);
}
