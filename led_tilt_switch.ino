const int tiltPin = 2;
const int ledPin  = 13;

void setup() {
  pinMode(tiltPin, INPUT_PULLUP); // internal pull-up
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int state = digitalRead(tiltPin); // HIGH = upright, LOW = tilted (with INPUT_PULLUP)
  if (state == LOW) {
    digitalWrite(ledPin, HIGH);
    Serial.println("Tilted!");
  } else {
    digitalWrite(ledPin, LOW);
    Serial.println("Upright");
  }
  delay(100);
}