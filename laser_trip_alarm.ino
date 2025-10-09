/*
  Laser Tripwire Alarm (Instant Reset, Passive Buzzer)
  Components:
    - LDR voltage divider: 5V -> LDR -> A0 -> 10k -> GND
    - Passive Buzzer: D8 -> + , - -> GND
    - LED: D13 -> resistor -> LED -> GND (optional)
    - Reset button: D7 to button to GND (INPUT_PULLUP)
*/

const int LDR_PIN = A0;
const int BUZZER_PIN = 8;
const int LED_PIN = 13;
const int RESET_PIN = 7;

const bool USE_TONE = true;       // true = passive buzzer
const bool AUTO_CALIBRATE = true; // auto threshold

int FIXED_THRESHOLD = 600;
const int AVG_SAMPLES = 5;
const unsigned int CONSECUTIVE_REQUIRED = 5;

unsigned int consecutiveBroken = 0;
bool alarmOn = false;
int thresholdVal = 600;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RESET_PIN, INPUT_PULLUP);

  Serial.begin(9600);
  delay(100);
  Serial.println("=== Laser Tripwire (Instant Reset) ===");

  if (AUTO_CALIBRATE) calibrateThreshold();
  else {
    thresholdVal = FIXED_THRESHOLD;
    Serial.print("Using fixed threshold = ");
    Serial.println(thresholdVal);
  }
}

void loop() {
  // Check reset first — works even during alarm
  if (digitalRead(RESET_PIN) == LOW && alarmOn) {
    resetAlarm();
    return;
  }

  int val = smoothRead();
  Serial.print("LDR: ");
  Serial.print(val);
  Serial.print("  thr=");
  Serial.println(thresholdVal);

  if (!alarmOn) {
    if (val < thresholdVal) {
      consecutiveBroken++;
      if (consecutiveBroken >= CONSECUTIVE_REQUIRED) {
        triggerAlarm();
      }
    } else {
      consecutiveBroken = 0;
    }
  } else {
    // Beeping loop (non-blocking)
    static unsigned long lastBeep = 0;
    static bool beepState = false;
    unsigned long now = millis();

    if (now - lastBeep > 150) {
      lastBeep = now;
      if (beepState) noTone(BUZZER_PIN);
      else tone(BUZZER_PIN, 2000);  // 2 kHz beep
      beepState = !beepState;
    }
  }

  delay(30);
}

int smoothRead() {
  long s = 0;
  for (int i = 0; i < AVG_SAMPLES; ++i) {
    s += analogRead(LDR_PIN);
    delay(4);
  }
  return s / AVG_SAMPLES;
}

void triggerAlarm() {
  Serial.println(">>> ALARM: beam broken!");
  alarmOn = true;
  digitalWrite(LED_PIN, HIGH);
}

void resetAlarm() {
  Serial.println("Alarm reset.");
  alarmOn = false;
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);
  consecutiveBroken = 0;
}

void calibrateThreshold() {
  Serial.println("Calibration mode.");
  Serial.println("1) Keep the laser ON and pointing at the LDR.");
  Serial.println("   Sampling 'beam ON' for 2 seconds...");
  long sumOn = 0;
  unsigned long end = millis() + 2000;
  int nOn = 0;
  while (millis() < end) {
    sumOn += analogRead(LDR_PIN);
    nOn++;
    delay(15);
  }
  int avgOn = sumOn / nOn;
  Serial.print("Avg (beam ON) = ");
  Serial.println(avgOn);

  Serial.println("2) Block the beam and press any key in Serial Monitor.");
  while (Serial.available() == 0) delay(50);
  while (Serial.available()) Serial.read();

  Serial.println("Sampling 'beam OFF' for 2 seconds...");
  long sumOff = 0;
  nOn = 0;
  end = millis() + 2000;
  while (millis() < end) {
    sumOff += analogRead(LDR_PIN);
    nOn++;
    delay(15);
  }
  int avgOff = sumOff / nOn;
  Serial.print("Avg (beam OFF) = ");
  Serial.println(avgOff);

  thresholdVal = (avgOn + avgOff) / 2;
  Serial.print("Auto threshold set to ");
  Serial.println(thresholdVal);
  Serial.println("Calibration complete. Start testing.");
}