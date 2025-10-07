#include <IRremote.h>

#define IR_RECEIVE_PIN 2
#define RED_PIN 3
#define GREEN_PIN 5
#define BLUE_PIN 6

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  Serial.println("Ready — use buttons 1–9 to control LED colors!");
}

void loop() {
  if (IrReceiver.decode()) {
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;
    Serial.print("Code: "); Serial.println(code, HEX);

    switch (code) {
      case 0xF30CFF00: setColor(255, 0, 0); break;   // 1 → Red
      case 0xE718FF00: setColor(0, 255, 0); break;   // 2 → Green
      case 0xA15EFF00: setColor(0, 0, 255); break;   // 3 → Blue
      case 0xF708FF00: setColor(255, 150, 0); break; // 4 → Yellow
      case 0xE31CFF00: setColor(0, 255, 255); break; // 5 → Cyan
      case 0xA55AFF00: setColor(255, 0, 255); break; // 6 → Magenta
      case 0xBD42FF00: setColor(255, 255, 255); break; // 7 → White
      case 0xAD52FF00: setColor(80, 80, 80); break;  // 8 → Dim white
      case 0xB54AFF00: setColor(0, 0, 0); break;     // 9 → OFF
      default: break;
    }

    IrReceiver.resume();
  }
}

void setColor(int r, int g, int b) {
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
}

