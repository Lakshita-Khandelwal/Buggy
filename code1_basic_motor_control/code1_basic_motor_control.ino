/*
 * ============================================================
 *  CODE 1 - BASIC MOTOR CONTROL
 * ============================================================
 *  Demonstrates all motor movement directions in sequence:
 *  forward, backward, left, right, clockwise, anticlockwise,
 *  and stop — each for 3 seconds.
 *
 *  PIN MAPPING:
 *   Motor A  IN1 → D5   IN2 → D6
 *   Motor B  IN3 → D7   IN4 → D8
 * ============================================================
 */

void setup() {
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
}

void forward() {
  digitalWrite(5, HIGH);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
}

void backward() {
  digitalWrite(5, LOW);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
}

void left() {
  digitalWrite(5, HIGH);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
}

void right() {
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
}

void clockwise() {
  digitalWrite(5, LOW);
  digitalWrite(6, HIGH);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
}

void anticlockwise() {
  digitalWrite(5, HIGH);
  digitalWrite(6, LOW);
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
}

void stopmotion() {
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
}

void loop() {
  forward();      delay(3000);
  backward();     delay(3000);
  left();         delay(3000);
  right();        delay(3000);
  clockwise();    delay(3000);
  anticlockwise();delay(3000);
  stopmotion();   delay(3000);
}
