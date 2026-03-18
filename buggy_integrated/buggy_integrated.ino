/*
 * ============================================================
 *  INTEGRATED AUTONOMOUS BUGGY CONTROLLER
 * ============================================================
 *  Features:
 *   - Line following via dual IR sensors (A0, A1)
 *   - Obstacle detection via HC-SR04 ultrasonic sensor
 *   - Gantry detection via RF pulse on pin 4
 *   - Full supervisory serial control (F/B/L/R/S/A/P commands)
 *   - Completes exactly 2 clockwise laps of the track
 *   - Safe parking on obstacle or manual command
 *   - Prints buggy state at every gantry and key event
 * ============================================================
 *  PIN MAPPING
 *   Motor A  IN1 → D5   IN2 → D6
 *   Motor B  IN3 → D7   IN4 → D8
 *   IR Left  → A0    IR Right → A1
 *   Ultrasonic TRIG → D13   ECHO → D12
 *   RF/Gantry pulse → D4
 * ============================================================
 */

// ── Motor pins ───────────────────────────────────────────────
#define IN1 5
#define IN2 6
#define IN3 7
#define IN4 8

// ── Sensor pins ──────────────────────────────────────────────
#define IR_LEFT   A0
#define IR_RIGHT  A1
#define TRIG_PIN  13
#define ECHO_PIN  12
#define RF_PIN    4

// ── Tuning constants ─────────────────────────────────────────
#define OBSTACLE_DIST_CM   20     // Stop if object closer than this
#define GANTRY_COUNT        3     // Total gantries per lap
#define TOTAL_LAPS          2     // Number of clockwise laps to complete
#define GANTRY_PAUSE_MS  1000     // Pause at each gantry (ms)
#define OBSTACLE_PAUSE_MS 2000    // Pause when obstacle detected (ms)
#define PARK_SETTLE_MS    500     // Settle delay after parking

// ── Gantry RF pulse windows (µs) ─────────────────────────────
#define GANTRY1_LOW  1500
#define GANTRY1_HIGH 2000
#define GANTRY2_LOW  2500
#define GANTRY2_HIGH 3000
#define GANTRY3_LOW   500
#define GANTRY3_HIGH 1000

// ── Mode flags ───────────────────────────────────────────────
//   MODE_AUTO   : full autonomous line-follow + obstacle avoid
//   MODE_MANUAL : serial supervisory control overrides motors
#define MODE_AUTO   0
#define MODE_MANUAL 1

// ── Global state ─────────────────────────────────────────────
int  mode        = MODE_AUTO;   // current operating mode
int  lapCount    = 0;           // completed laps
int  gantryCount = 0;           // gantries passed this lap
bool parked      = false;       // true when safely parked
bool obstacleHeld = false;      // true while obstacle is blocking
long duration;
int  distance;

// ─────────────────────────────────────────────────────────────
//  MOTOR PRIMITIVES
// ─────────────────────────────────────────────────────────────
void motorStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void forward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}

void backward() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

// Gentle left curve  – left motor slower (off), right motor on
void turnLeft() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, LOW);
}

// Gentle right curve – right motor slower (off), left motor on
void turnRight() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}

// Pivot clockwise in place
void pivotClockwise() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}

// Pivot anti-clockwise in place
void pivotAntiClockwise() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

// ─────────────────────────────────────────────────────────────
//  ULTRASONIC DISTANCE
// ─────────────────────────────────────────────────────────────
int readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30 ms timeout
  if (duration == 0) return 999;             // no echo → clear
  return (int)(duration * 0.0343 / 2.0);
}

// ─────────────────────────────────────────────────────────────
//  GANTRY DETECTION  (RF pulse width)
// ─────────────────────────────────────────────────────────────
// Returns 0 if no gantry, 1/2/3 for the matching gantry
int detectGantry() {
  if (digitalRead(RF_PIN) == LOW) return 0;
  unsigned long pw = pulseIn(RF_PIN, HIGH, 5000); // 5 ms timeout
  if (pw == 0) return 0;
  if (pw > GANTRY1_LOW && pw < GANTRY1_HIGH) return 1;
  if (pw > GANTRY2_LOW && pw < GANTRY2_HIGH) return 2;
  if (pw > GANTRY3_LOW && pw < GANTRY3_HIGH) return 3;
  return 0;
}

// ─────────────────────────────────────────────────────────────
//  STATUS PRINTING
// ─────────────────────────────────────────────────────────────
void printTrackState(const char* event) {
  int l = digitalRead(IR_LEFT);
  int r = digitalRead(IR_RIGHT);
  int d = readDistanceCM();

  Serial.println(F("╔══════════════════════════════╗"));
  Serial.println(F("║      BUGGY STATUS REPORT     ║"));
  Serial.println(F("╠══════════════════════════════╣"));

  Serial.print  (F("║ Event      : ")); Serial.println(event);
  Serial.print  (F("║ Lap        : ")); Serial.print(lapCount);
  Serial.print  (F(" / ")); Serial.println(TOTAL_LAPS);
  Serial.print  (F("║ Gantry #   : ")); Serial.println(gantryCount);
  Serial.print  (F("║ IR Left    : ")); Serial.println(l ? "ON LINE" : "OFF LINE");
  Serial.print  (F("║ IR Right   : ")); Serial.println(r ? "ON LINE" : "OFF LINE");
  Serial.print  (F("║ Distance   : ")); Serial.print(d); Serial.println(F(" cm"));
  Serial.print  (F("║ Mode       : ")); Serial.println(mode == MODE_AUTO ? "AUTO" : "MANUAL");
  Serial.print  (F("║ Parked     : ")); Serial.println(parked ? "YES" : "NO");
  Serial.println(F("╚══════════════════════════════╝"));
  Serial.println();
}

void printObstacleWarning(int dist) {
  Serial.print(F("[OBSTACLE] Object detected at "));
  Serial.print(dist);
  Serial.println(F(" cm — buggy stopped."));
}

void printLapComplete() {
  Serial.print(F("[LAP] Lap "));
  Serial.print(lapCount);
  Serial.println(F(" complete!"));
  if (lapCount >= TOTAL_LAPS) {
    Serial.println(F("[MISSION] 2 laps completed. Parking buggy."));
  }
}

// ─────────────────────────────────────────────────────────────
//  SAFE PARK
// ─────────────────────────────────────────────────────────────
void safePark(const char* reason) {
  motorStop();
  parked = true;
  Serial.print(F("[PARK] Buggy parked. Reason: "));
  Serial.println(reason);
  printTrackState("PARKED");
}

// ─────────────────────────────────────────────────────────────
//  GANTRY HANDLER
//  Called each time a gantry is detected
// ─────────────────────────────────────────────────────────────
void handleGantry(int g) {
  motorStop();
  gantryCount++;

  // Every time we pass gantry 1 after the start, we completed a lap
  // Track layout: gantries are 1 → 2 → 3 per lap
  if (g == GANTRY_COUNT) {        // passed the last gantry = lap done
    lapCount++;
    printLapComplete();
    gantryCount = 0;              // reset per-lap gantry counter
  }

  char label[32];
  snprintf(label, sizeof(label), "GANTRY %d", g);
  printTrackState(label);

  delay(GANTRY_PAUSE_MS);

  // After 2 laps, park permanently
  if (lapCount >= TOTAL_LAPS) {
    safePark("2 laps completed");
  }
}

// ─────────────────────────────────────────────────────────────
//  AUTONOMOUS LINE-FOLLOWING STEP
//  Called every loop iteration when in AUTO mode
// ─────────────────────────────────────────────────────────────
void autoStep() {
  // 1. Obstacle check
  distance = readDistanceCM();
  if (distance <= OBSTACLE_DIST_CM) {
    if (!obstacleHeld) {
      obstacleHeld = true;
      printObstacleWarning(distance);
      printTrackState("OBSTACLE DETECTED");
    }
    motorStop();
    delay(OBSTACLE_PAUSE_MS);
    return;                       // wait until clear
  }
  obstacleHeld = false;           // obstacle gone, resume

  // 2. Gantry check
  int g = detectGantry();
  if (g > 0) {
    handleGantry(g);
    return;                       // gantry handling already called motor functions
  }

  // 3. Line following (IR sensors)
  //    Sensor logic: HIGH = on black line, LOW = off line
  int l = digitalRead(IR_LEFT);
  int r = digitalRead(IR_RIGHT);

  if      (l == 1 && r == 1) { forward();    }   // both on line → go forward
  else if (l == 0 && r == 1) { turnLeft();   }   // drifted right → steer left
  else if (l == 1 && r == 0) { turnRight();  }   // drifted left → steer right
  else                        { forward();   }    // both off → continue forward
}

// ─────────────────────────────────────────────────────────────
//  SERIAL SUPERVISORY CONTROL
//  Commands:
//    F – forward       B – backward
//    L – left          R – right
//    C – clockwise     A – anticlockwise
//    S – stop          P – safe park
//    M – toggle auto/manual mode
//    ? – print status
// ─────────────────────────────────────────────────────────────
void handleSerial() {
  if (!Serial.available()) return;
  char cmd = (char)Serial.read();

  // Mode toggle is always available
  if (cmd == 'M') {
    mode = (mode == MODE_AUTO) ? MODE_MANUAL : MODE_AUTO;
    Serial.print(F("[CMD] Mode → "));
    Serial.println(mode == MODE_AUTO ? "AUTO" : "MANUAL");
    if (mode == MODE_AUTO) parked = false; // resume auto
    return;
  }

  if (cmd == '?') {
    printTrackState("MANUAL QUERY");
    return;
  }

  // In AUTO mode only P (park) and M are accepted
  if (mode == MODE_AUTO) {
    if (cmd == 'P') { safePark("manual command"); }
    else {
      Serial.println(F("[CMD] In AUTO mode. Send 'M' to switch to MANUAL, 'P' to park."));
    }
    return;
  }

  // ── MANUAL mode commands ─────────────────────────────────
  parked = false;   // any manual movement unparks
  switch (cmd) {
    case 'F': forward();           Serial.println(F("[CMD] Forward"));        break;
    case 'B': backward();          Serial.println(F("[CMD] Backward"));       break;
    case 'L': turnLeft();          Serial.println(F("[CMD] Left"));           break;
    case 'R': turnRight();         Serial.println(F("[CMD] Right"));          break;
    case 'C': pivotClockwise();    Serial.println(F("[CMD] Clockwise"));      break;
    case 'A': pivotAntiClockwise();Serial.println(F("[CMD] Anti-clockwise")); break;
    case 'S': motorStop();         Serial.println(F("[CMD] Stop"));           break;
    case 'P': safePark("manual command");                                      break;
    default:
      Serial.print(F("[CMD] Unknown: ")); Serial.println(cmd);
      Serial.println(F("  Valid: F B L R C A S P M ?"));
  }
}

// ─────────────────────────────────────────────────────────────
//  SETUP
// ─────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  pinMode(IR_LEFT,  INPUT);
  pinMode(IR_RIGHT, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RF_PIN,   INPUT);

  motorStop();

  Serial.println(F("============================================"));
  Serial.println(F("  INTEGRATED BUGGY CONTROLLER - READY"));
  Serial.println(F("============================================"));
  Serial.println(F("  Mode    : AUTO (line-follow + obstacle)"));
  Serial.println(F("  Mission : 2 clockwise laps"));
  Serial.println(F("  Send 'M' to toggle MANUAL/AUTO mode"));
  Serial.println(F("  Send '?' for status at any time"));
  Serial.println(F("============================================"));
  Serial.println();

  printTrackState("STARTUP");
  delay(2000);    // brief ready-pause before starting
}

// ─────────────────────────────────────────────────────────────
//  MAIN LOOP
// ─────────────────────────────────────────────────────────────
void loop() {
  // Always handle serial input first (highest priority)
  handleSerial();

  // If parked, do nothing until operator intervenes
  if (parked) {
    return;
  }

  // Autonomous mode
  if (mode == MODE_AUTO) {
    autoStep();
  }
  // Manual mode: motors are driven entirely by serial commands;
  // no autonomous step needed here.
}
