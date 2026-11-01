# Rover

Small 2-wheel Arduino car controlled over Bluetooth.

## Hardware

- Arduino Uno
- L298N motor driver + 2 DC motors
- HC-05/HC-06 bluetooth module
- HC-SR04 ultrasonic sensor (wired, not used yet)

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

- `F` forward
- `B` backward
- `L` left
- `R` right
- `S` stop
