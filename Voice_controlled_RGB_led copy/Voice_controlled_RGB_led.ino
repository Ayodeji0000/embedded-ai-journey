// 🎤 Voice-Controlled RGB LED (Customizable Yellow)
// Common Cathode RGB LED
// Pins: R=4, G=3, B=2
// Commands from Python: 'R','G','B','Y','W','O'

const int redPin   = 4;
const int greenPin = 3;
const int bluePin  = 2;

// ✨ Customizable yellow levels (adjust these!)
int yellowRed   = 255;  // try between 150–255
int yellowGreen = 180;  // try between 100–255

void setup() {
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  setColor(0, 0, 0); // start with LED off
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();

    switch (command) {
      case 'R': setColor(255, 0, 0); break;
      case 'G': setColor(0, 255, 0); break;
      case 'B': setColor(0, 0, 255); break;
      case 'Y': setColor(225, 150, 0); break; //
      case 'W': setColor(255, 255, 255); break;
      case 'O': setColor(0, 0, 0); break; // off
    }
  }
}

void setColor(int r, int g, int b) {
  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}