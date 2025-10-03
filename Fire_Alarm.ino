#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin definitions
const int flameSensorPin = 2; // Digital input from flame sensor
const int ledPin = 8;         // Red LED
const int buzzerPin = 9;      // Passive buzzer control (via transistor)

// LCD setup (I2C)
LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x27 is common I2C address, 16x2 LCD

void setup() {
  pinMode(flameSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  Serial.begin(9600);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0,0);
  lcd.print("SYSTEM READY");
  lcd.setCursor(0,1);
  lcd.print("No flame detected");
  
  Serial.println("🔥 Flame Sensor Fire Alarm Initialized");
}

void loop() {
  int flameState = digitalRead(flameSensorPin);
  
  if (flameState == LOW) { // Flame detected
    digitalWrite(ledPin, HIGH);       // Turn on LED
    tone(buzzerPin, 1000);            // Passive buzzer ON at 1kHz
    
    Serial.println("🔥 Flame Detected! Alarm ON");
    
    // Update LCD
    lcd.setCursor(0,0);
    lcd.print("!!! FIRE DETECTED");   // Attention-grabbing
    lcd.setCursor(0,1);
    lcd.print("ALARM ACTIVE      "); // Pad spaces to clear old text
    
  } else { // No flame
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);                // Passive buzzer OFF
    
    Serial.println("No flame detected.");
    
    // Update LCD
    lcd.setCursor(0,0);
    lcd.print("SYSTEM READY      ");  // Pad spaces to clear old text
    lcd.setCursor(0,1);
    lcd.print("No flame detected");
  }
  
  delay(200); // Short delay for stability
}