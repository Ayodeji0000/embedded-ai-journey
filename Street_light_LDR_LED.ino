int LDR = A0;       // LDR connected to analog pin A0
int LED = 9;        // LED on digital pin 9
int threshold = 200; // Adjust this value based on your room lighting

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600); // To see LDR values in Serial Monitor
}

void loop() {
  int lightValue = analogRead(LDR); // Read LDR value (0-1023)
  Serial.println(lightValue);       // Print for debugging

  if (lightValue < threshold) {     // If dark
    digitalWrite(LED, HIGH);        // LED ON
  } else {
    digitalWrite(LED, LOW);         // LED OFF
  }

  delay(200); // Small delay
}