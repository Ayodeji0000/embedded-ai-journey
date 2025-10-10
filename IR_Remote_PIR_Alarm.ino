#include <IRremote.hpp>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ==== Pin Mapping ====
#define PIR_PIN 3
#define IR_PIN 2
#define BUZZER_PIN 8
// =====================

bool systemArmed = false;
bool settingPIN = true;   // true at startup (forces first setup)
bool alarmActive = false;
String enteredPIN = "";
String storedPIN = "";

// for non-blocking buzzer
unsigned long lastBuzzTime = 0;
bool buzzerState = false;

// ===== Remote Button Codes =====
const uint32_t POWER_CODE = 0xBA45FF00; // POWER button
const uint32_t MODE_CODE  = 0xB946FF00; // MODE button

// Number buttons 0–9
uint32_t NUM_CODES[10] = {
  0xE916FF00, // 0
  0xF30CFF00, // 1
  0xE718FF00, // 2
  0xA15EFF00, // 3
  0xF708FF00, // 4
  0xE31CFF00, // 5
  0xA55AFF00, // 6
  0xBD42FF00, // 7
  0xAD52FF00, // 8
  0xB54AFF00  // 9
};
// ===============================

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
  lcd.init();
  lcd.backlight();

  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AYO Smart Alarm");
  delay(1500);
  lcd.clear();
  lcd.print("Set 3-digit PIN:");
}

void loop() {
  // === Always listen for IR input ===
  if (IrReceiver.decode()) {
    uint32_t code = IrReceiver.decodedIRData.decodedRawData;
    Serial.print("IR: "); Serial.println(code, HEX);
    handleRemoteInput(code);
    IrReceiver.resume();
  }

  // === PIR motion detection ===
  if (systemArmed && !alarmActive && digitalRead(PIR_PIN) == HIGH) {
    alarmActive = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Motion Detected!");
    lcd.setCursor(0, 1);
    lcd.print("Enter PIN:");
  }

  // === Alarm buzzer (non-blocking, digital toggle) ===
  if (alarmActive && systemArmed) {
    unsigned long now = millis();
    if (now - lastBuzzTime > 200) { // toggle every 200ms
      lastBuzzTime = now;
      buzzerState = !buzzerState;
      digitalWrite(BUZZER_PIN, buzzerState);
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void handleRemoteInput(uint32_t code) {
  // === MODE resets PIN (only when disarmed) ===
  if (!systemArmed && code == MODE_CODE) {
    settingPIN = true;
    storedPIN = "";
    enteredPIN = "";
    lcd.clear();
    lcd.print("Set New PIN:");
    return;
  }

  // === PIN setup ===
  if (settingPIN) {
    handlePINSetup(code);
    return;
  }

  // === ARM ===
  if (code == POWER_CODE && !alarmActive) {
    systemArmed = true;
    lcd.clear();
    lcd.print("System Armed");
    delay(1000);
    lcd.clear();
    lcd.print("Monitoring...");
    enteredPIN = "";
    return;
  }

  // === DISARM ===
  if (systemArmed) {
    handleDisarmInput(code);
  }
}

void handlePINSetup(uint32_t code) {
  char num = getNumberFromCode(code);
  if (num == 'X') return;

  enteredPIN += num;
  lcd.setCursor(0, 1);
  lcd.print(enteredPIN); // show visible digits during setup

  // === Wait a moment before reacting so LCD shows all 3 digits ===
  if (enteredPIN.length() == 3) {
    delay(300); // ensure 3rd digit is visible
    storedPIN = enteredPIN;
    enteredPIN = "";
    settingPIN = false;
    lcd.clear();
    lcd.print("PIN Saved!");
    delay(1200);
    lcd.clear();
    lcd.print("System Disarmed");
  }
}

void handleDisarmInput(uint32_t code) {
  char num = getNumberFromCode(code);
  if (num == 'X') return;

  enteredPIN += num;

  // show asterisks for entered digits
  lcd.setCursor(0, 1);
  lcd.print("            ");
  lcd.setCursor(0, 1);
  for (int i = 0; i < enteredPIN.length(); i++) {
    lcd.print('*');
  }

  // === Wait a moment to display all 3 stars ===
  if (enteredPIN.length() == 3) {
    delay(300); // ensures third * appears
    if (enteredPIN == storedPIN) {
      // correct PIN → disarm
      alarmActive = false;
      systemArmed = false;
      digitalWrite(BUZZER_PIN, LOW);
      lcd.clear();
      lcd.print("Correct PIN!");
      delay(1000);
      lcd.clear();
      lcd.print("System Disarmed");
      delay(1000);
      lcd.clear();
      lcd.print("Press POWER");
      lcd.setCursor(0, 1);
      lcd.print("to Arm again");
    } else {
      lcd.clear();
      lcd.print("Wrong PIN!");
      delay(1000);
      lcd.clear();
      if (alarmActive)
        lcd.print("Enter PIN:");
      else
        lcd.print("System Armed");
    }
    enteredPIN = "";
  }
}

char getNumberFromCode(uint32_t code) {
  for (int i = 0; i < 10; i++) {
    if (code == NUM_CODES[i]) return '0' + i;
  }
  return 'X';
}