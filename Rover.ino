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

  void setSpeed(uint8_t s) {
    speed = s;
    if (running) analogWrite(en, speed); // apply immediately while moving
  }
  void forward()  { drive(true); }
  void backward() { drive(false); }

  void stop() {
    running = false;
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(en, 0);
  }

  // active brake: short the motor through the driver instead of coasting
  void brake() {
    running = false;
    digitalWrite(in1, HIGH);
    digitalWrite(in2, HIGH);
    analogWrite(en, 255);
  }

private:
  uint8_t in1, in2, en, speed = 150;
  bool running = false;

  void drive(bool a) {
    running = true;
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
  void forward()           { left.forward(); right.forward(); }
  void backward()          { left.backward(); right.backward(); }
  void turnRight()         { left.backward(); right.forward(); }
  void turnLeft()          { left.forward(); right.backward(); }
  void stop()              { left.stop(); right.stop(); }
  void brake()             { left.brake(); right.brake(); }

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

    long d = pulseIn(echo, HIGH, 6000); // ~1 m max, keeps loop fast
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

enum Motion { STOP, FWD, BACK, LEFT, RIGHT };

const int SPEED_MIN = 100, SPEED_MAX = 255, SPEED_STEP = 25;
const unsigned long TURN_MS = 200;  // duration of one L/R tap
const unsigned long PING_MS = 60;   // sonar interval while moving forward
const long STOP_CM = 30;            // obstacle distance that blocks forward

Motion motion = STOP;
Motion prevMotion = STOP;           // motion to resume after a turn tap
unsigned long turnEnd = 0;
unsigned long lastPing = 0;
int speed = 150;

// median of 3 readings, so one noisy echo can't hide or fake an obstacle
bool pathClear() {
  long a = sonar.readCm(); delayMicroseconds(500);
  long b = sonar.readCm(); delayMicroseconds(500);
  long c = sonar.readCm();
  long m = max(min(a, b), min(max(a, b), c));
  return m >= STOP_CM;
}

void apply(Motion m) {
  if (m == FWD && !pathClear()) m = STOP; // never drive into an obstacle
  motion = m;
  switch (m) {
    case FWD:   car.forward();   break;
    case BACK:  car.backward();  break;
    case LEFT:  car.turnLeft();  break;
    case RIGHT: car.turnRight(); break;
    default:    car.stop();      break;
  }
  if (m == FWD) lastPing = millis();
}

void turn(Motion m) {
  if (motion != LEFT && motion != RIGHT) prevMotion = motion;
  apply(m);
  turnEnd = millis() + TURN_MS; // repeated taps extend the turn
}

void changeSpeed(int delta) {
  speed = constrain(speed + delta, SPEED_MIN, SPEED_MAX);
  car.setSpeed(speed);
}

void setup() {
  car.begin();
  sonar.begin();
  bt.begin(9600);

  // left/right PWM offset to compensate for motor mismatch
  car.setSpeed(speed);
}

void loop() {
  if (bt.available()) {
    char cmd = (char)bt.read();
    switch (cmd) {
      case 'F': apply(FWD);   break;
      case 'B': apply(BACK);  break;
      case 'S': apply(STOP);  break;
      case 'L': turn(LEFT);   break;
      case 'R': turn(RIGHT);  break;
      case '+': changeSpeed(SPEED_STEP);  break;
      case '-': changeSpeed(-SPEED_STEP); break;
      default: break;
    }
  }

  unsigned long now = millis();

  // end of a turn tap: resume what we were doing before it
  if ((motion == LEFT || motion == RIGHT) && (long)(now - turnEnd) >= 0) {
    apply(prevMotion);
  }

  // while moving forward, stop if something is in front
  if (motion == FWD && now - lastPing >= PING_MS) {
    lastPing = now;
    if (!pathClear()) { car.brake(); motion = STOP; }
  }
}
