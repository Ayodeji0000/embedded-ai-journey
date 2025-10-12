// occupancy_logger.ino
// Sends CSV lines over Serial: millis,light, motion, label
// Pinout: LDR -> A0 (with 10k to GND), PIR -> D2, Button -> D3 (pullup)

const int LDR_PIN = A0;
const int PIR_PIN = 2;
const int BUTTON_PIN = 3;

const unsigned long INTERVAL_MS = 1000; // send every 1 second
unsigned long lastMillis = 0;

void setup() {
  Serial.begin(9600);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // button to GND when pressed
  // Print CSV header once
  delay(500);
  Serial.println("time_ms,light, motion, label");
}

void loop() {
  unsigned long now = millis();
  if (now - lastMillis >= INTERVAL_MS) {
    lastMillis = now;

    int lightRaw = analogRead(LDR_PIN); // 0-1023
    int motion = digitalRead(PIR_PIN);  // 0 or 1
    int buttonState = digitalRead(BUTTON_PIN); // HIGH (not pressed) or LOW (pressed)
    int label = (buttonState == LOW) ? 1 : 0; // press button while present to label occupied

    // Print CSV line (no spaces)
    Serial.print(now);
    Serial.print(',');
    Serial.print(lightRaw);
    Serial.print(',');
    Serial.print(motion);
    Serial.print(',');
    Serial.println(label);
  }
}