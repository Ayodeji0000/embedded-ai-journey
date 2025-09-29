// Stopwatch with Start/Stop + Reset (2 buttons)
// Pin mapping: a=2, b=3, c=4, d=5, e=6, f=7, g=8
const int segPins[7] = {2, 3, 4, 5, 6, 7, 8}; // a,b,c,d,e,f,g
const int dpPin = 9;  
const int startStopBtn = 10;
const int resetBtn = 11;

const uint8_t digits[10] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

bool running = false;
int currentNumber = 0;
unsigned long lastUpdate = 0;
unsigned long interval = 1000; // 1 sec

void setup() {
  for (int i = 0; i < 7; i++) pinMode(segPins[i], OUTPUT);
  pinMode(dpPin, OUTPUT);
  
  pinMode(startStopBtn, INPUT_PULLUP);
  pinMode(resetBtn, INPUT_PULLUP);

  clearDisplay();
}

void clearDisplay() {
  for (int i = 0; i < 7; i++) digitalWrite(segPins[i], LOW);
}

void displayDigit(int num) {
  if (num < 0 || num > 9) return;
  uint8_t p = digits[num];
  for (int i = 0; i < 7; i++) {
    bool on = (p >> i) & 0x01;
    digitalWrite(segPins[i], on ? HIGH : LOW);
  }
}

void loop() {
  // Start/Stop button
  if (digitalRead(startStopBtn) == LOW) {
    delay(50);
    while (digitalRead(startStopBtn) == LOW);
    running = !running;
  }

  // Reset button
  if (digitalRead(resetBtn) == LOW) {
    delay(50);
    while (digitalRead(resetBtn) == LOW);
    currentNumber = 0;
    displayDigit(currentNumber);
    running = false; // also stop when reset
  }

  // Count when running
  if (running && (millis() - lastUpdate >= interval)) {
    lastUpdate = millis();
    currentNumber = (currentNumber + 1) % 10;
    displayDigit(currentNumber);
  }
}