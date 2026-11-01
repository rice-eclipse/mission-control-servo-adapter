# Mission Control Servo Adapter

## About

This system takes two 12V-24V DC outputs from our Mission Control box and translates them into two 5V servo PWM outputs, for use on our Sphinx liquid engine. It uses the AVR64DU32T-I/PT (IC1), TQFP-32.

## Pinout

| Pin | Port | Net | Direction | Connects to | Notes |
| --- | --- | --- | --- | --- | --- |
| 1 | PA3 | LED1 | Output | D1 via R5 (1.5k) | Active high |
| 2 | PA4 | LED2 | Output | D2 via R6 (1.5k) | Active high |
| 3 | PA5 | LED3 | Output | D3 via R9 (1.5k) | Active high |
| 7 | DM | DM | USB | USB1 D- via R13 (22R) | |
| 8 | DP | DP | USB | USB1 D+ via R12 (22R) | |
| 9 | PC3 | BOOT | Input | SW1 | Active low, 10k pull-up (R14) |
| 11 | PD1 | INPUT_1_SCALED | Analog input | J4 pin 1 via divider | 56k/10k (R1/R7), 100nF filter (C1) |
| 12 | PD2 | INPUT_2_SCALED | Analog input | J5 pin 1 via divider | 56k/10k (R3/R8), 100nF filter (C2) |
| 27 | PF7 | UPDI | Programming | J7 pin 1 | |
| 31 | PA1 | SERVO_1 | PWM output | J1 pin 1 | |
| 32 | PA2 | SERVO_2 | PWM output | J2 pin 1 | |