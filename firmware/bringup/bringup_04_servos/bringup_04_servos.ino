// Bring-up 04: Servo PWM
//
// Checks the 50 Hz servo signals on SERVO_1 (PA1, TCA0 WO1) and
// SERVO_2 (PA2, TCA0 WO2).
//
// SAFETY: disconnect the servos from any valve linkage before running this.
// It sweeps both servos through their full range.
//
// Power: servos must run from J3, not the UPDI Friend. Before powering J3,
// disconnect the UPDI Friend's PWR wire from J7 pin 2 (leave UPDI and GND).
// Otherwise J3 back-feeds your PC's USB port through the programmer.
//
// Expected:
//   1. Both servos go to center (1500 us) and hold for 3 s. LED1 is on.
//   2. Both sweep slowly from 1000 us to 2000 us and back, in opposite
//      directions, so you can tell the channels apart. LED2 is on while the
//      pulse is rising, LED3 while it's falling.
// With a scope: 20 ms period, 1000-2000 us high time, 5 V high level.
//
// TCA0 is driven directly, so the core's analogWrite() on PA1/PA2
// isn't used here.

#if defined(MILLIS_USE_TIMERA0)
#error "millis() uses TCA0. Pick a TCB timer in Tools > millis()/micros() Timer."
#endif

const uint8_t LED1 = PIN_PA3;
const uint8_t LED2 = PIN_PA4;
const uint8_t LED3 = PIN_PA5;

const uint16_t PULSE_MIN_US = 1000;
const uint16_t PULSE_CENTER_US = 1500;
const uint16_t PULSE_MAX_US = 2000;
const uint16_t SWEEP_STEP_US = 5;
const uint16_t SWEEP_STEP_MS = 20;  // One step per PWM frame: 1000 us in 4 s

// TCA0 clocked at F_CPU / 16 (1.5 MHz at 24 MHz).
const uint32_t TCA_HZ = F_CPU / 16;
const uint16_t TCA_PERIOD = TCA_HZ / 50 - 1;

uint16_t usToTicks(uint16_t us) {
  return (uint32_t)us * (TCA_HZ / 1000) / 1000;
}

void setServoPulses(uint16_t servo1Us, uint16_t servo2Us) {
  // Buffered registers update at the end of the current frame, so a
  // change never produces a short or stretched pulse.
  TCA0.SINGLE.CMP1BUF = usToTicks(servo1Us);
  TCA0.SINGLE.CMP2BUF = usToTicks(servo2Us);
}

void setupServoPwm() {
  takeOverTCA0();

  PORTMUX.TCAROUTEA = (PORTMUX.TCAROUTEA & ~PORTMUX_TCA0_gm) | PORTMUX_TCA0_PORTA_gc;
  pinMode(PIN_PA1, OUTPUT);
  pinMode(PIN_PA2, OUTPUT);

  TCA0.SINGLE.CTRLA = 0;
  TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_CMP2EN_bm;
  TCA0.SINGLE.PER = TCA_PERIOD;
  TCA0.SINGLE.CMP1 = usToTicks(PULSE_CENTER_US);
  TCA0.SINGLE.CMP2 = usToTicks(PULSE_CENTER_US);
  TCA0.SINGLE.CNT = 0;
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc | TCA_SINGLE_ENABLE_bm;
}

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  setupServoPwm();

  digitalWrite(LED1, HIGH);
  delay(3000);
  digitalWrite(LED1, LOW);
}

void loop() {
  digitalWrite(LED2, HIGH);
  for (uint16_t us = PULSE_MIN_US; us <= PULSE_MAX_US; us += SWEEP_STEP_US) {
    setServoPulses(us, PULSE_MIN_US + PULSE_MAX_US - us);
    delay(SWEEP_STEP_MS);
  }
  digitalWrite(LED2, LOW);

  digitalWrite(LED3, HIGH);
  for (uint16_t us = PULSE_MAX_US; us >= PULSE_MIN_US; us -= SWEEP_STEP_US) {
    setServoPulses(us, PULSE_MIN_US + PULSE_MAX_US - us);
    delay(SWEEP_STEP_MS);
  }
  digitalWrite(LED3, LOW);
}
