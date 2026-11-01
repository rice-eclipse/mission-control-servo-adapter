# Mission Control Servo Adapter

## MCU Pinout

MCU: AVR64DU32T-I/PT (IC1), TQFP-32

### Signals

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

### Power

| Pin | Name | Net | Notes |
| --- | --- | --- | --- |
| 6 | VUSB | - | 1uF to GND (C8) |
| 18 | VDD | +5V | 100nF decoupling (C10) |
| 19 | GND | GND | |
| 28 | VDD | +5V | 100nF decoupling (C9) |
| 29 | GND | GND | |

### Unused

These pins are not connected:

| Pin | Port |
| --- | --- |
| 4 | PA6 |
| 5 | PA7 |
| 10 | PD0 |
| 13 | PD3 |
| 14 | PD4 |
| 15 | PD5 |
| 16 | PD6 |
| 17 | PD7 |
| 20 | PF0 (XTAL32K1) |
| 21 | PF1 (XTAL32K2) |
| 22 | PF2 |
| 23 | PF3 |
| 24 | PF4 |
| 25 | PF5 |
| 26 | PF6 (RESET) |
| 30 | PA0 (XTALHF1) |
