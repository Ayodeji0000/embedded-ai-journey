int ledR = 3;
int ledG = 5;
int ledB = 6;
int buttonPin = 2;

int currentColor = 0; // 0=off, 1=yellow, 2=blue, 3=green, 4=red, 5=white
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(buttonPin, INPUT); // using external pull-down
  setColor(0); // LED off initially
}

void loop() {
  static int lastButtonState = LOW;
  int reading = digitalRead(buttonPin);

  // debounce
  if (reading != lastButtonState) lastDebounceTime = millis();
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == HIGH) { // button pressed with pull-down
      currentColor++;
      if (currentColor > 5) currentColor = 0; // off after last color
      setColor(currentColor);
      delay(200); // prevent multiple triggers
    }
  }
  lastButtonState = reading;
}

// Set RGB PWM values for each color
void setColor(int colorIndex) {
  int r=0, g=0, b=0;
  switch(colorIndex){
    case 1: r=255; g=150; b=0; break;        // Yellow (adjusted for visibility)
    case 2: r=0; g=0; b=255; break;         // Blue
    case 3: r=0; g=50; b=0; break;          // Green
    case 4: r=255; g=0; b=0; break;         // Red
    case 5: r=255; g=255; b=255; break;      // White
    default: r=0; g=0; b=0; break;          // Off
  }
  analogWrite(ledR, constrain(r,0,255));
  analogWrite(ledG, constrain(g,0,255));
  analogWrite(ledB, constrain(b,0,255));
}