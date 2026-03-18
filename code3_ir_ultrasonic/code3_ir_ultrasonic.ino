/*
 * ============================================================
 *  CODE 3 - IR LINE FOLLOWING + ULTRASONIC OBSTACLE DETECTION
 * ============================================================
 *  Combines IR-based line following with an HC-SR04 ultrasonic
 *  sensor. If an obstacle is detected within 20 cm, the buggy
 *  stops for 2 seconds before resuming line following.
 *
 *  PIN MAPPING:
 *   IR Left        → A0
 *   IR Right       → A1
 *   Ultrasonic TRIG → D13
 *   Ultrasonic ECHO → D12
 *   Motor A  IN1   → D5    IN2 → D6
 *   Motor B  IN3   → D7    IN4 → D8
 * ============================================================
 */

const int trigPin = 13;
const int echoPin = 12;

long duration;
int  distance;
int  l, r;

void forward() {
  digitalWrite(5, HIGH);
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

void right() {
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
}

void Stop() {
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
}

void setup() {
  pinMode(A0,      INPUT);
  pinMode(A1,      INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(5,       OUTPUT);
  pinMode(6,       OUTPUT);
  pinMode(7,       OUTPUT);
  pinMode(8,       OUTPUT);
}

void loop() {
  l = digitalRead(A0);
  r = digitalRead(A1);

  // Ultrasonic measurement
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0343 / 2;

  if (distance <= 20) {
    // Obstacle detected — stop and wait
    Stop();
    delay(2000);
  } else {
    // No obstacle — follow line
    if (l == 1 && r == 1) { forward(); }
    if (l == 0 && r == 1) { left();    }
    if (l == 1 && r == 0) { right();   }
    if (l == 0 && r == 0) { forward(); }
  }
}
