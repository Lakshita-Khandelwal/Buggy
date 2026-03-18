/*
 * ============================================================
 *  CODE 4 - IR LINE FOLLOWING + ULTRASONIC + RF GANTRY DETECT
 * ============================================================
 *  Extends Code 3 with RF pulse-width gantry detection on D4.
 *  When a gantry pulse is detected, the buggy stops briefly
 *  and prints which gantry was crossed over Serial.
 *  Resumes line-following after each gantry stop.
 *
 *  Gantry Pulse Windows (microseconds):
 *   Gantry 1 → 1500 – 2000 µs
 *   Gantry 2 → 2500 – 3000 µs
 *   Gantry 3 →  500 – 1000 µs
 *
 *  PIN MAPPING:
 *   IR Left        → A0
 *   IR Right       → A1
 *   RF / Gantry    → D4
 *   Ultrasonic TRIG → D13
 *   Ultrasonic ECHO → D12
 *   Motor A  IN1   → D5    IN2 → D6
 *   Motor B  IN3   → D7    IN4 → D8
 * ============================================================
 */

int ir1 = A0;
int ir2 = A1;
int pin = 4;
int l, r;

unsigned long value = 0;

const int trigPin = 13;
const int echoPin = 12;

long duration;
int  distance;

void setup() {
  Serial.begin(9600);
  pinMode(ir1,     INPUT);
  pinMode(ir2,     INPUT);
  pinMode(pin,     INPUT);
  pinMode(5,       OUTPUT);
  pinMode(6,       OUTPUT);
  pinMode(7,       OUTPUT);
  pinMode(8,       OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

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

void loop() {
  // ── Gantry detection ─────────────────────────────────────
  if (digitalRead(pin) > 0) {
    value = pulseIn(pin, HIGH);
    Serial.print("Value: ");
    Serial.println(value);

    if (value > 1500 && value < 2000) {
      Serial.println("Gantry 1 crossed.");
      Stop();
      delay(1000);
    }
    if (value > 2500 && value < 3000) {
      Serial.println("Gantry 2 crossed.");
      Stop();
      delay(1000);
    }
    if (value > 500 && value < 1000) {
      Serial.println("Gantry 3 crossed.");
      Stop();
      delay(1000);
    }
  }

  // ── Read IR sensors ──────────────────────────────────────
  l = digitalRead(A0);
  r = digitalRead(A1);

  // ── Ultrasonic measurement ───────────────────────────────
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0343 / 2;

  if (distance <= 20) {
    // Obstacle detected
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
