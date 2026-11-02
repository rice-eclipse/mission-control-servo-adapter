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
## Bring-up

Bring-up sketches are in [firmware/bringup](firmware/bringup). Run them in order, since each one assumes the previous one passed.

| Sketch | Tests | Needs |
| --- | --- | --- |
| [bringup_01_leds](firmware/bringup/bringup_01_leds/bringup_01_leds.ino) | MCU runs, clock, LED1-3 | UPDI Friend only |
| [bringup_02_boot_button](firmware/bringup/bringup_02_boot_button/bringup_02_boot_button.ino) | SW1 and R14 on PC3 | SW1 fitted |
| [bringup_03_inputs](firmware/bringup/bringup_03_inputs/bringup_03_inputs.ino) | Input dividers and ADC | Bench supply, 0-24V, on J4/J5 |
| [bringup_04_servos](firmware/bringup/bringup_04_servos/bringup_04_servos.ino) | 50 Hz servo PWM on J1/J2 | 5V on J3, servos off any linkage |

Each sketch's header comment says what the LEDs should do when it passes.

### Arduino setup

The board is programmed with [DxCore](https://github.com/SpenceKonde/DxCore). AVR DU support isn't in a DxCore release yet (1.6.2 doesn't include it), so install DxCore manually from the GitHub repository rather than through Boards Manager.

Tools menu settings (exact menu names may differ in the unreleased DU build):

- Board: the AVR DU-series entry without a bootloader, chip AVR64DU32
- Clock: 24 MHz internal
- Flash mapping (FLMAP): unlocked. The default lock setting has been reported to hang the AVR64DU32 after upload.
- millis()/micros() Timer: any TCB timer (`bringup_04_servos` refuses to build if millis uses TCA0)
- Programmer: SerialUPDI, 230400 baud

Upload with Sketch > Upload Using Programmer.

### Wiring the UPDI Friend

Set the UPDI Friend's voltage switch to 5V, then wire it to J7 by pin label:

| UPDI Friend | J7 |
| --- | --- |
| UPDI | 1 (UPDI) |
| PWR | 2 (+5V) |
| GND | 3 (GND) |

With nothing else connected, the UPDI Friend powers the board from your PC's USB port. That's enough for tests 01-03.

Don't power the board from J3 while the UPDI Friend's PWR wire is connected. J3, J7 pin 2 and the USB connector's VBUS are all on the same +5V net, so J3 would back-feed the PC's USB port. For test 04, disconnect the PWR wire and leave UPDI and GND connected.

Never plug anything into USB1. Its VBUS is tied directly to +5V.

## Firmware

The main firmware is [firmware/servo_adapter](firmware/servo_adapter/servo_adapter.ino). It uses the same Arduino setup as the bring-up sketches.

| Input | State | Servo | LED |
| --- | --- | --- | --- |
| J4 (INPUT_1) | 0V | J1 (SERVO_1) to position 1 (off / safe) | LED1 off |
| J4 (INPUT_1) | 12V or 24V | J1 (SERVO_1) to position 2 | LED1 on |
| J5 (INPUT_2) | 0V | J2 (SERVO_2) to position 1 (off / safe) | LED2 off |
| J5 (INPUT_2) | 12V or 24V | J2 (SERVO_2) to position 2 | LED2 on |

A disconnected input reads as 0V, so its servo goes to position 1. Both servos start in position 1 at power-up and move to position 2 about 20 ms later if their input is on.

An input turns on above 8V and off below 4V, and must hold its new state for 20 ms before the servo moves.

LED3 is the status LED:

| LED3 | Meaning |
| --- | --- |
| 1 Hz blink | Running normally |
| Fast blink | An input went above 30V (stays fast for 1 s after) |
| Off or solid | Firmware not running |

Set the servo end points at the top of the sketch: `servo1Pos1`, `servo1Pos2`, `servo2Pos1` and `servo2Pos2`. They default to 1000 us and 2000 us. Tune them with the servos off the linkage first.
