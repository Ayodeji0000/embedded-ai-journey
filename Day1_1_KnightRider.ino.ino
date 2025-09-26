// Pin assignments
int redLED = 2;
int greenLED = 3;
int blueLED = 4;
int yellowLED = 5;

int leds[] = {2, 3, 4, 5}; // array of LED pins
int numLeds = 4;

void setup() {
  for (int i = 0; i < numLeds; i++) {
    pinMode(leds[i], OUTPUT);
  }
}

void loop() {
  // Forward direction
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(leds[i], HIGH);
    delay(200);
    digitalWrite(leds[i], LOW);
  }

  // Backward direction
  for (int i = numLeds - 2; i > 0; i--) { 
    digitalWrite(leds[i], HIGH);
    delay(200);
    digitalWrite(leds[i], LOW);
  }
}