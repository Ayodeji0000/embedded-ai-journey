// Advanced Police Siren + Irregular Red & Blue Flashes with Button
const int buzzer = 8;       // Passive buzzer pin
const int blueLED = 9;      // Blue pin of RGB LED
const int redLED = 10;      // Red pin of RGB LED
const int button = 2;       // Button pin

bool buzzerOn = false;
bool buttonPressed = false;

// Siren sweep variables
int freq = 400;         
int freqStep = 15;      
bool sweepUp = true;    
unsigned long previousSirenTime = 0;
unsigned long sirenInterval = 5; // base speed

// LED flashing variables
unsigned long previousLEDTime = 0;
unsigned long ledInterval = 200; // initial flash interval
bool ledState = false;           // which LED is on

void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(button, INPUT_PULLUP); // Button connected to GND

  digitalWrite(redLED, LOW);
  digitalWrite(blueLED, LOW);
  randomSeed(analogRead(A0)); // seed random for intervals
}

void loop() {
  unsigned long currentMillis = millis();

  // --- Button toggle ---
  if (digitalRead(button) == LOW) {
    if (!buttonPressed) {
      buzzerOn = !buzzerOn;
      buttonPressed = true;
      if (!buzzerOn) {
        noTone(buzzer);
        digitalWrite(blueLED, LOW);  
        digitalWrite(redLED, LOW);   
      }
    }
  } else {
    buttonPressed = false;
  }

  // --- Siren sweep ---
  if (buzzerOn && (currentMillis - previousSirenTime >= sirenInterval)) {
    previousSirenTime = currentMillis;

    // Update frequency
    if (sweepUp) {
      freq += freqStep;
      if (freq >= 1500) sweepUp = false;
    } else {
      freq -= freqStep;
      if (freq <= 400) sweepUp = true;
    }

    // Slightly vary siren interval for realism
    sirenInterval = 5 + random(0, 5);

    // Update buzzer
    tone(buzzer, freq);
  }

  // --- Irregular alternating police LED flashing ---
  if (buzzerOn && (currentMillis - previousLEDTime >= ledInterval)) {
    previousLEDTime = currentMillis;
    ledState = !ledState;

    digitalWrite(redLED, ledState ? HIGH : LOW);
    digitalWrite(blueLED, ledState ? LOW : HIGH);

    // Randomize next flash interval for realism
    ledInterval = 150 + random(0, 100); // 150ms to 250ms
  }
}