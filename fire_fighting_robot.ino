/*
  Autonomous Fire-Fighting Robot
  --------------------------------
  Al-Nahrain University Graduation Projects Exhibition (May-Jun 2025) — 2nd place
  Author: Muwafaq Salar Saadi

  DESCRIPTION:
  Three IR flame sensors (left, center, right) scan for a flame source.
  When a flame is detected, the robot steers toward it using the L298 motor
  driver, and once close enough, activates a water pump (via a TIP-122
  transistor switch) to extinguish the fire. A mini servo can optionally aim
  the pump nozzle.

  HARDWARE:
    - Arduino Uno
    - 3x IR flame sensors (digital output, active LOW when flame detected)
    - L298 motor driver + 4x BO motors (2 left-side, 2 right-side, wired in parallel per side)
    - Mini servo (pump/nozzle aiming)
    - 5-9V water pump, driven by a TIP-122 transistor (with 1K base resistor
      and a 104pF capacitor for switching noise suppression)
    - 2x 3.7V 18650 batteries (motor/pump power, separate from Arduino logic power)

  WIRING (adjust pins to match your actual build):
    Flame sensor LEFT   -> D2
    Flame sensor CENTER -> D3
    Flame sensor RIGHT  -> D4

    L298 IN1 -> D5   (Left motors forward)
    L298 IN2 -> D6   (Left motors backward)
    L298 IN3 -> D7   (Right motors forward)
    L298 IN4 -> D8   (Right motors backward)
    L298 ENA -> D9   (Left side speed, PWM)
    L298 ENB -> D10  (Right side speed, PWM)

    Servo signal -> D11
    Pump transistor base (via 1K resistor) -> D12
*/

#include <Servo.h>

// ---- Pin assignments ----
const int FLAME_LEFT   = 2;
const int FLAME_CENTER = 3;
const int FLAME_RIGHT  = 4;

const int MOTOR_LEFT_FWD  = 5;
const int MOTOR_LEFT_BWD  = 6;
const int MOTOR_RIGHT_FWD = 7;
const int MOTOR_RIGHT_BWD = 8;
const int MOTOR_LEFT_EN   = 9;
const int MOTOR_RIGHT_EN  = 10;

const int SERVO_PIN = 11;
const int PUMP_PIN  = 12;

// ---- Tuning ----
const int DRIVE_SPEED       = 180;   // 0-255 PWM speed while seeking flame
const int TURN_SPEED        = 140;   // 0-255 PWM speed while turning
const int PUMP_RUN_MS       = 4000;  // how long to run the pump once triggered
const int FLAME_CONFIRM_MS  = 300;   // flame must be seen this long to confirm (debounce)

Servo nozzleServo;

void setup() {
  pinMode(FLAME_LEFT, INPUT);
  pinMode(FLAME_CENTER, INPUT);
  pinMode(FLAME_RIGHT, INPUT);

  pinMode(MOTOR_LEFT_FWD, OUTPUT);
  pinMode(MOTOR_LEFT_BWD, OUTPUT);
  pinMode(MOTOR_RIGHT_FWD, OUTPUT);
  pinMode(MOTOR_RIGHT_BWD, OUTPUT);
  pinMode(MOTOR_LEFT_EN, OUTPUT);
  pinMode(MOTOR_RIGHT_EN, OUTPUT);

  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);

  nozzleServo.attach(SERVO_PIN);
  nozzleServo.write(90); // centered

  stopMotors();

  Serial.begin(9600);
  Serial.println("Fire-fighting robot initialized.");
}

void loop() {
  bool flameLeft   = flameDetected(FLAME_LEFT);
  bool flameCenter  = flameDetected(FLAME_CENTER);
  bool flameRight  = flameDetected(FLAME_RIGHT);

  if (!flameLeft && !flameCenter && !flameRight) {
    // No flame seen: idle in place. (Optional: replace with a search/patrol
    // routine, e.g. slow rotation, if the competition arena requires active search.)
    stopMotors();
    return;
  }

  if (flameCenter) {
    // Flame roughly ahead: drive straight toward it.
    driveForward(DRIVE_SPEED);
  } else if (flameLeft) {
    // Flame to the left: turn left.
    turnLeft(TURN_SPEED);
  } else if (flameRight) {
    // Flame to the right: turn right.
    turnRight(TURN_SPEED);
  }

  delay(150); // brief step, then re-read sensors (simple proportional-ish approach)

  // Once centered and close (in practice: use a distance/IR proximity sensor
  // or simply time-based approach here), stop and extinguish.
  if (flameCenter) {
    stopMotors();
    extinguish();
  }
}

bool flameDetected(int pin) {
  // Flame sensors used here are active LOW: LOW = flame detected.
  return digitalRead(pin) == LOW;
}

void driveForward(int speed) {
  digitalWrite(MOTOR_LEFT_FWD, HIGH);
  digitalWrite(MOTOR_LEFT_BWD, LOW);
  digitalWrite(MOTOR_RIGHT_FWD, HIGH);
  digitalWrite(MOTOR_RIGHT_BWD, LOW);
  analogWrite(MOTOR_LEFT_EN, speed);
  analogWrite(MOTOR_RIGHT_EN, speed);
}

void turnLeft(int speed) {
  digitalWrite(MOTOR_LEFT_FWD, LOW);
  digitalWrite(MOTOR_LEFT_BWD, HIGH);
  digitalWrite(MOTOR_RIGHT_FWD, HIGH);
  digitalWrite(MOTOR_RIGHT_BWD, LOW);
  analogWrite(MOTOR_LEFT_EN, speed);
  analogWrite(MOTOR_RIGHT_EN, speed);
}

void turnRight(int speed) {
  digitalWrite(MOTOR_LEFT_FWD, HIGH);
  digitalWrite(MOTOR_LEFT_BWD, LOW);
  digitalWrite(MOTOR_RIGHT_FWD, LOW);
  digitalWrite(MOTOR_RIGHT_BWD, HIGH);
  analogWrite(MOTOR_LEFT_EN, speed);
  analogWrite(MOTOR_RIGHT_EN, speed);
}

void stopMotors() {
  digitalWrite(MOTOR_LEFT_FWD, LOW);
  digitalWrite(MOTOR_LEFT_BWD, LOW);
  digitalWrite(MOTOR_RIGHT_FWD, LOW);
  digitalWrite(MOTOR_RIGHT_BWD, LOW);
  analogWrite(MOTOR_LEFT_EN, 0);
  analogWrite(MOTOR_RIGHT_EN, 0);
}

void extinguish() {
  Serial.println("Flame confirmed at close range. Activating pump.");
  nozzleServo.write(90); // aim forward; adjust/sweep as needed for your build
  digitalWrite(PUMP_PIN, HIGH);
  delay(PUMP_RUN_MS);
  digitalWrite(PUMP_PIN, LOW);
  Serial.println("Pump cycle complete.");
  delay(1000); // brief pause before resuming search
}
