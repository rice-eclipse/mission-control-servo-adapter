// Bring-up 03: Inputs
//
// Checks the J4/J5 input dividers and the ADC on PD1 (AIN1) and PD2 (AIN2).
// There's no serial port on this board, so the result is shown on the LEDs.
//
// Each divider is 56k over 10k, so the pin sees Vin * 10 / 66.
// With VDD (5 V) as the ADC reference, full scale is about 33 V at the input.
//
// Expected, using a bench supply on J4 pin 1 / J5 pin 1 (pin 2 is GND):
//   - LED1 on when INPUT_1 is above ON_VOLTS, off when below OFF_VOLTS.
//   - LED2 does the same for INPUT_2.
//   - LED3 blinks to show the loop is running.
// Try 0 V, 12 V and 24 V on each input. Both should switch at about the
// same voltage. If the thresholds look wrong, check R1/R7 and R3/R8.
//
// The ADC reference is the 5 V rail, so the measured voltage moves with it.
// That's fine for on/off detection, but don't rely on it for precise readings.

const uint8_t LED1 = PIN_PA3;
const uint8_t LED2 = PIN_PA4;
const uint8_t LED3 = PIN_PA5;
const uint8_t INPUT_1 = PIN_PD1;
const uint8_t INPUT_2 = PIN_PD2;

const float VREF = 5.0;
const float DIVIDER = (56.0 + 10.0) / 10.0;
const float ADC_MAX = 255.0;  // 8-bit; DxCore 1.6.2's 10-bit mode on the AVR DU divides readings by 4

// Hysteresis, so a slowly changing input doesn't make the LED flicker.
const float ON_VOLTS = 8.0;
const float OFF_VOLTS = 4.0;

bool input1On = false;
bool input2On = false;

float readInputVolts(uint8_t pin) {
  return analogRead(pin) * (VREF / ADC_MAX) * DIVIDER;
}

bool applyHysteresis(bool currentlyOn, float volts) {
  if (currentlyOn) {
    return volts > OFF_VOLTS;
  }
  return volts > ON_VOLTS;
}

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  analogReference(VDD);
  analogReadResolution(8);
}

void loop() {
  input1On = applyHysteresis(input1On, readInputVolts(INPUT_1));
  input2On = applyHysteresis(input2On, readInputVolts(INPUT_2));

  digitalWrite(LED1, input1On);
  digitalWrite(LED2, input2On);
  digitalWrite(LED3, (millis() / 250) % 2);

  delay(10);
}
