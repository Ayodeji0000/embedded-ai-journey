// External pull-down button -> toggle LED
const int buttonPin = 2;   // button input (external pull-down)
const int ledPin    = 13;  // LED output

int ledState = LOW;
int buttonState = LOW;
int lastButtonState = LOW;

void setup() {
  pinMode(buttonPin, INPUT); // external pull-down is used
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  Serial.begin(9600);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  Serial.println(buttonState); // 0 when not pressed, 1 when pressed

  // detect rising edge: LOW -> HIGH
  if (buttonState == HIGH && lastButtonState == LOW) {
    ledState = !ledState;               // flip LED state
    digitalWrite(ledPin, ledState);     // apply it
    delay(50);                          // debounce (ignore bounces)
  }

  lastButtonState = buttonState;
  delay(10);
}