#include <SoftwareSerial.h>

class Motor {
public:
  Motor(uint8_t in1, uint8_t in2, uint8_t en)
    : in1(in1), in2(in2), en(en) {}

  void begin() {
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(en, OUTPUT);
    analogWrite(en, speed);
    stop();
  }

  void setSpeed(uint8_t s) { speed = s; }
  void forward()  { drive(true); }
  void backward() { drive(false); }

  void stop() {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(en, 0);
  }

private:
  uint8_t in1, in2, en, speed = 150;

  void drive(bool a) {
    digitalWrite(in1, !a ? HIGH : LOW);
    digitalWrite(in2, !a ? LOW : HIGH);
    analogWrite(en, speed);
  }
};

class Car {
public:
  Car(Motor& left, Motor& right) : left(left), right(right) {}

  void begin() { left.begin(); right.begin(); }

  void setSpeed(uint8_t s) { left.setSpeed(s); right.setSpeed(s - 50); }
  void forward()           { stop(); delay(100); left.forward(); right.forward(); }
  void backward()          { stop(); delay(100); left.backward(); right.backward(); }
  void turnRight()         { stop(); delay(100); left.backward(); right.forward(); }
  void turnLeft()          { stop(); delay(100); left.forward(); right.backward(); }
  void stop()              { left.stop(); right.stop(); }

private:
  Motor& left;
  Motor& right;
};

class Ultrasonic {
public:
  Ultrasonic(uint8_t trig, uint8_t echo) : trig(trig), echo(echo) {}

  void begin() {
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
  }

  long readCm() {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    long d = pulseIn(echo, HIGH, 30000);
    if (d == 0) return 999; // nothing in range
    return d * 0.034 / 2;
  }

private:
  uint8_t trig, echo;
};

// =========================================================
Motor rightMotor(13, 12, 11);
Motor leftMotor(8, 7, 6);
Car car(leftMotor, rightMotor);
Ultrasonic sonar(2, 3);
SoftwareSerial bt(9, 4);

void setup() {
  car.begin();
  sonar.begin();
  bt.begin(9600);

  // left/right PWM offset to compensate for motor mismatch
  car.setSpeed(200);
}

void loop() {
  if (bt.available()) {
    char cmd = (char)bt.read();
    switch (cmd) {
      case 'F':
        car.forward();
        break;
      case 'B':
        car.backward();
        break;
      case 'L':
        car.turnLeft();
        break;
      case 'R':
        car.turnRight();
        break;
      case 'S':
        car.stop();
        break;
      default:
        break;
    }
  }
}
