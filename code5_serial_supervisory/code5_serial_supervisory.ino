/*
 * ============================================================
 *  CODE 5 - SERIAL SUPERVISORY CONTROL
 * ============================================================
 *  Allows full manual control of the buggy over Serial
 *  (USB or Bluetooth HC-05/HC-06 at 9600 baud).
 *  Each command moves the buggy and prints the action taken.
 *
 *  Serial Commands:
 *   F → Forward
 *   B → Backward
 *   L → Left
 *   R → Right
 *   S → Stop
 *
 *  PIN MAPPING:
 *   Motor A  IN1 → D5   IN2 → D6
 *   Motor B  IN3 → D7   IN4 → D8
 * ============================================================
 */

int flag = 0;

void setup() {
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.begin(9600);
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
  digitalWrite(5, LOW);
  digitalWrite(6, HIGH);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
}

void right() {
  digitalWrite(5, HIGH);
  digitalWrite(6, LOW);
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
}

void stopMotors() {
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();

    if (command == 'F') {
      forward();
      flag = 1;
      Serial.println("Forward");
    } else if (command == 'B') {
      backward();
      flag = 1;
      Serial.println("Backward");
    } else if (command == 'L') {
      left();
      flag = 1;
      Serial.println("Left");
    } else if (command == 'R') {
      right();
      flag = 1;
      Serial.println("Right");
    } else if (command == 'S') {
      stopMotors();
      flag = 0;
      Serial.println("Stopped");
    }
  }
}
