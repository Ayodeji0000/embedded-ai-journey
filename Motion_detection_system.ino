#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const uint8_t LCD_ADDR = 0x27;
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

const int pirPin    = 2;
const int buzzerPin = 3;
const int ledPins[] = {4, 5, 6, 7};
const int buttonPin = 8;   // Pushbutton with INPUT_PULLUP

bool alarmActive = false;
unsigned long lastBeepToggle = 0;
bool buzzerOn = false;
const unsigned long BEEP_INTERVAL = 300; // ms on/off

unsigned long alarmEndTime = 0;
const unsigned long ALARM_DURATION = 10000; // 10 seconds

bool waitForPirLow = false;  // PIR must go LOW before new trigger
bool pirLastState = LOW;     // previous PIR reading
unsigned long rebootStartTime = 0;
const unsigned long MAX_REBOOT_TIME = 15000; // max 15s countdown

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(ledPins[i], OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("PIR Alarm Ready");
  lcd.setCursor(0, 1);
  lcd.print("No Motion");

  Serial.begin(9600);
}

void setAllLeds(bool on) {
  for (int i = 0; i < 4; i++) digitalWrite(ledPins[i], on ? HIGH : LOW);
}

void startAlarm() {
  setAllLeds(true);
  alarmActive = true;
  alarmEndTime = millis() + ALARM_DURATION;
}

void stopAlarm() {
  setAllLeds(false);
  noTone(buzzerPin);
  buzzerOn = false;
  alarmActive = false;
}

void handleBuzzerBeep() {
  if (alarmActive) {
    unsigned long now = millis();
    if (now - lastBeepToggle >= BEEP_INTERVAL) {
      lastBeepToggle = now;
      if (buzzerOn) {
        noTone(buzzerPin);
        buzzerOn = false;
      } else {
        tone(buzzerPin, 2000);
        buzzerOn = true;
      }
    }
  }
}

void loop() {
  unsigned long now = millis();
  int pirState = digitalRead(pirPin);
  int btnState = digitalRead(buttonPin);

  // Button pressed → stop alarm and start reboot
  if (btnState == LOW && alarmActive) {
    delay(50); // debounce
    if (digitalRead(buttonPin) == LOW) {
      stopAlarm();
      waitForPirLow = true;
      rebootStartTime = now;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Alarm Silenced");
      Serial.println("Button pressed -> Alarm OFF, system rebooting");
      delay(250);
    }
  }

  // Wait for PIR LOW → show countdown
  if (waitForPirLow) {
    unsigned long elapsed = now - rebootStartTime;
    long remaining = (long)MAX_REBOOT_TIME - (long)elapsed;
    if (remaining < 0) remaining = 0;

    lcd.setCursor(0, 1);
    lcd.print("Rebooting: ");
    lcd.print(remaining / 1000 + 1); // round up to seconds
    lcd.print(" s   "); // clear leftover chars

    if (pirState == LOW) {
      waitForPirLow = false;
      pirLastState = LOW;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("System Ready");
      lcd.setCursor(0, 1);
      lcd.print("Waiting Motion");
      Serial.println("PIR LOW detected -> ready for new motion");
    } else {
      handleBuzzerBeep();
      return; // skip normal PIR detection
    }
  }

  // Rising-edge detection: trigger alarm only on LOW→HIGH transition
  if (pirState == HIGH && pirLastState == LOW && !alarmActive) {
    startAlarm();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MOTION DETECTED");
    Serial.println("Motion -> Alarm ON for 10s");
  }

  // Update countdown display if alarm is active
  if (alarmActive) {
    unsigned long timeLeft = (alarmEndTime > now) ? (alarmEndTime - now) / 1000 : 0;
    lcd.setCursor(0, 1);
    lcd.print("Alarm: ");
    lcd.print(timeLeft);
    lcd.print(" sec   "); // clear previous numbers
  }

  // Stop alarm after 10s
  if (alarmActive && now >= alarmEndTime) {
    stopAlarm();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No Motion");
    lcd.setCursor(0, 1);
    lcd.print("Alarm OFF");
    Serial.println("10s expired -> Alarm OFF");
  }

  pirLastState = pirState; // update last PIR state
  handleBuzzerBeep();
  delay(20);
}