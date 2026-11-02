// Bring-up 01: LEDs
//
// Checks that the MCU runs, the clock is right, and all three LEDs work.
//
// Expected:
//   1. LED1, LED2, LED3 each light for 500 ms, in order.
//   2. All three light together for 500 ms.
//   3. All off for 500 ms, then repeat.
// One full cycle takes 2.5 s. If it runs noticeably faster or slower,
// the clock setting in Tools > Clock is wrong.

const uint8_t LED_PINS[] = {PIN_PA3, PIN_PA4, PIN_PA5};  // LED1, LED2, LED3
const uint8_t LED_COUNT = sizeof(LED_PINS);

void setAll(uint8_t level) {
  for (uint8_t i = 0; i < LED_COUNT; i++) {
    digitalWrite(LED_PINS[i], level);
  }
}

void setup() {
  for (uint8_t i = 0; i < LED_COUNT; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
}

void loop() {
  for (uint8_t i = 0; i < LED_COUNT; i++) {
    digitalWrite(LED_PINS[i], HIGH);
    delay(500);
    digitalWrite(LED_PINS[i], LOW);
  }

  setAll(HIGH);
  delay(500);
  setAll(LOW);
  delay(500);
}
