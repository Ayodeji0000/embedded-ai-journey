#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// DHT11 setup
#define DHTPIN 2        // DHT11 data pin connected to Arduino D2
#define DHTTYPE DHT11   // Sensor type
DHT dht(DHTPIN, DHTTYPE);

// LCD setup
#define I2C_ADDR 0x27   // Change to 0x3F if scanner finds that instead
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

unsigned long lastRead = 0;
const unsigned long interval = 2000; // DHT11 needs ~2s between reads

void setup() {
  Serial.begin(9600);
  dht.begin();

  // Initialize LCD
  lcd.init();       
  lcd.backlight();  

  // Startup message
  lcd.setCursor(0,0);
  lcd.print("Weather Station");
  lcd.setCursor(0,1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
}

void loop() {
  if (millis() - lastRead < interval) return;
  lastRead = millis();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Celsius

  // Check if readings failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT11!");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Sensor error!");
    return;
  }

  // Debug in Serial Monitor
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" *C  ");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Display on LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(temperature, 1);
  lcd.print((char)223); // Degree symbol
  lcd.print("C");

  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(humidity, 1);
  lcd.print("%");
}