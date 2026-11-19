# Rover

Small 2-wheel Arduino car controlled over Bluetooth.

## Hardware

- Arduino Uno
- L298N motor driver + 2 DC motors
- HC-05/HC-06 bluetooth module
- HC-SR04 ultrasonic sensor (stops the car before obstacles)

## Pins

| Part          | Pins                    |
|---------------|-------------------------|
| Left motor    | IN1 8, IN2 7, EN 6      |
| Right motor   | IN1 13, IN2 12, EN 11   |
| Bluetooth     | RX 9, TX 4              |
| Ultrasonic    | TRIG 2, ECHO 3          |

## Build

Needs `arduino-cli` with the `arduino:avr` core.

```
make compile-and-upload
make monitor
```

Port is `/dev/ttyACM0`, change it in the Makefile if yours is different.

## Commands

Send these over bluetooth (9600 baud):

- `F` forward (ignored if something is closer than 12 cm)
- `B` backward
- `L` left tap: turns for 200 ms, then resumes the previous motion
- `R` right tap: same as `L`, other way
- `S` stop
- `+` speed up (+25, max 255)
- `-` slow down (-25, min 100)

## Behaviour

- Tap `L`/`R` repeatedly to turn further. A tap while driving forward works as a steering correction.
- While moving forward the sonar is checked every 20 ms and the car stops if an obstacle is within 12 cm and brakes (not coasts). `B`, `L` and `R` still work so you can back out.
- The right motor always runs 50 PWM below the left to compensate for motor mismatch. Speed starts at 150.
- Tunables (`TURN_MS`, `STOP_CM`, `SPEED_*`) are constants at the top of the control section in `Rover.ino`.
