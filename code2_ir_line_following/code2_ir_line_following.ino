/*
 * ============================================================
 *  CODE 2 - IR SENSOR LINE FOLLOWING
 * ============================================================
 *  Uses two IR sensors (left and right) to follow a line.
 *  Sensor reads HIGH when on the line, LOW when off the line.
 *
 *  Logic:
 *   L=1, R=1 → Both on line      → go forward
 *   L=0, R=1 → Drifted right     → steer left
 *   L=1, R=0 → Drifted left      → steer right
 *   L=0, R=0 → Both off line     → go forward (best guess)
 *
 *  PIN MAPPING:
 *   IR Left  → A0    IR Right → A1
 *   Motor A  IN1 → D5   IN2 → D6
 *   Motor B  IN3 → D7   IN4 → D8
 * ============================================================
 */

int l, r;

void setup() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
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

void right() {
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
}

void left() {
  digitalWrite(5, HIGH);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
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

void loop() {
  l = digitalRead(A0);
  r = digitalRead(A1);

  if (l == 1 && r == 1) { forward(); }
  if (l == 0 && r == 1) { left();    }
  if (l == 1 && r == 0) { right();   }
  if (l == 0 && r == 0) { forward(); }
}
