#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int trigPin = 7;
const int echoPin = 6;
const int ledPin  = 5;
const int buzzerPin = 10;

LiquidCrystal_I2C lcd(0x27, 16, 2); // common addresses: 0x27 or 0x3F

// thresholds (cm)
const int warningDist = 40;   // LED on below this
const int criticalDist = 20;  // Buzzer active below this

unsigned long lastLCD = 0;
const unsigned long lcdInterval = 200; // update LCD every 200 ms

// buzzer timing (non-blocking)
unsigned long lastBeepToggle = 0;
bool buzzerOn = false;
int currentBeepInterval = 500; // ms between beep toggles

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(trigPin, LOW);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0); lcd.print("Ultrasonic Ready");
  delay(800);
  lcd.clear();
}

float getDistanceCM() {
  // send 10us pulse to trigger
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // read echo pulse
  unsigned long duration = pulseIn(echoPin, HIGH, 30000UL); // timeout 30ms
  if (duration == 0) return -1.0; // no echo / out of range

  // convert to cm: speed of sound ~343 m/s -> 29.1 us per cm (round trip)
  float distanceCm = (duration / 29.1) / 2.0;
  return distanceCm;
}

void loop() {
  float dist = getDistanceCM(); // cm, -1 if out of range
  unsigned long now = millis();

  // determine state
  bool inWarning = false;
  bool inCritical = false;
  if (dist > 0) {
    if (dist <= warningDist) inWarning = true;
    if (dist <= criticalDist) inCritical = true;
  }

  // LED control (warning)
  digitalWrite(ledPin, inWarning ? HIGH : LOW);

  // Buzzer control (critical) - non-blocking variable beep rate
  if (inCritical) {
    // map distance to beep interval: closer => faster beeps
    // distance range: 0..criticalDist -> interval 80ms..500ms
    int d = max(0, (int)min(dist, (float)criticalDist));
    currentBeepInterval = map(d, 0, criticalDist, 80, 500);
    if (currentBeepInterval < 80) currentBeepInterval = 80;

    if (now - lastBeepToggle >= (unsigned long)currentBeepInterval) {
      lastBeepToggle = now;
      buzzerOn = !buzzerOn;
      if (buzzerOn) {
        // play tone (frequency 1000 Hz). tone() works for passive and active buzzers.
        tone(buzzerPin, 1000);
      } else {
        noTone(buzzerPin);
      }
    }
  } else {
    // make sure buzzer is off
    noTone(buzzerPin);
    buzzerOn = false;
  }

  // LCD update (every lcdInterval)
  if (now - lastLCD >= lcdInterval) {
    lastLCD = now;
    lcd.setCursor(0,0);
    if (dist < 0) {
      lcd.print("Out of range     ");
    } else {
      lcd.print("Dist: ");
      lcd.print(dist, 1);
      lcd.print(" cm    ");
    }

    lcd.setCursor(0,1);
    if (inCritical) {
      lcd.print("STATUS: PROXIMITY ");
    } else if (inWarning) {
      lcd.print("STATUS: WARNING   ");
    } else {
      lcd.print("STATUS: OK        ");
    }
  }

  // small short sleep to avoid hammering sensor too fast
  delay(50);
}