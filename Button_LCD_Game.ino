/*
  LCD Runner Game (I2C Version - Live Score Update)
  -------------------------------------------------
  - 16x2 I2C LCD
  - Jump button: D7 (press to jump)
  - Reset button: D8 (press to restart after game over)
  - Uses custom LCD sprites for player and obstacle
  - Score & High score saved in EEPROM
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 
// If nothing appears, change 0x27 → 0x3F

// Buttons
const int JUMP_PIN = 7;
const int RESET_PIN = 8;

// Game logic
const int PLAYER_COL = 2;
int obstacleCol = 15;

unsigned long lastFrame = 0;
unsigned long frameDelay = 220;

bool jumping = false;
int jumpCounter = 0;
const int JUMP_FRAMES = 3;

int score = 0;
int highScore = 0;
bool gameOver = false;

const int EEPROM_ADDR = 0;

// Custom sprites (5x8 pixel bitmaps)
byte playerGroundChar[8] = {
  0b00100,
  0b01110,
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b01010,
  0b10001
};

byte playerAirChar[8] = {
  0b00100,
  0b00100,
  0b01110,
  0b01010,
  0b00100,
  0b01100,
  0b10000,
  0b00000
};

byte obstacleChar[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b00100,
  0b00100,
  0b00100,
  0b00100
};

void setup() {
  pinMode(JUMP_PIN, INPUT_PULLUP);
  pinMode(RESET_PIN, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  // Load custom characters
  lcd.createChar(0, playerGroundChar);
  lcd.createChar(1, playerAirChar);
  lcd.createChar(2, obstacleChar);

  // Load high score
  EEPROM.get(EEPROM_ADDR, highScore);
  if (highScore < 0 || highScore > 10000) highScore = 0;

  showIntro();
  delay(800);
  resetGame();
}

void loop() {
  unsigned long now = millis();

  if (digitalRead(RESET_PIN) == LOW && gameOver) {
    resetGame();
    delay(250);
  }

  if (!gameOver && (now - lastFrame >= frameDelay)) {
    lastFrame = now;

    // Handle jump
    if (digitalRead(JUMP_PIN) == LOW && !jumping) {
      jumping = true;
      jumpCounter = JUMP_FRAMES;
    }

    if (jumping) {
      jumpCounter--;
      if (jumpCounter <= 0) jumping = false;
    }

    // Move obstacle
    obstacleCol--;
    if (obstacleCol < 0) {
      obstacleCol = 15;
      score++;
      updateScoreDisplay(); // update score live on LCD
      if (score % 5 == 0 && frameDelay > 80) frameDelay -= 15;
    }

    // Collision detection
    if (obstacleCol == PLAYER_COL && !jumping) {
      gameOver = true;
      if (score > highScore) {
        highScore = score;
        EEPROM.put(EEPROM_ADDR, highScore);
      }
    }

    drawGame();
  }

  if (gameOver) delay(50);
}

void drawGame() {
  if (gameOver) {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("GAME OVER");
    lcd.setCursor(1, 1);
    lcd.print("S:");
    lcd.print(score);
    lcd.print("  Hi:");
    lcd.print(highScore);
    return;
  }

  // Don’t clear the whole screen (prevents flicker)
  // Only update the bottom line for movement and top-right for score

  // Draw player
  if (jumping) {
    lcd.setCursor(PLAYER_COL, 0);
    lcd.write(byte(1)); // playerAir
    lcd.setCursor(PLAYER_COL, 1);
    lcd.print(' '); // clear below
  } else {
    lcd.setCursor(PLAYER_COL, 0);
    lcd.print(' '); // clear above
    lcd.setCursor(PLAYER_COL, 1);
    lcd.write(byte(0)); // playerGround
  }

  // Clear previous obstacle position
  lcd.setCursor((obstacleCol + 1) % 16, 1);
  lcd.print(' ');

  // Draw obstacle
  if (obstacleCol >= 0 && obstacleCol < 16) {
    lcd.setCursor(obstacleCol, 1);
    lcd.write(byte(2));
  }
}

void resetGame() {
  obstacleCol = 15;
  score = 0;
  frameDelay = 220;
  jumping = false;
  jumpCounter = 0;
  gameOver = false;
  lcd.clear();
  drawHUD();
  drawGame();
}

void drawHUD() {
  // Draw initial score display on top right
  lcd.setCursor(12, 0);
  lcd.print("S:");
  lcd.print(score);
}

void updateScoreDisplay() {
  lcd.setCursor(14, 0);
  if (score < 10) lcd.print(" ");
  lcd.setCursor(13, 0);
  lcd.print(score);
}

void showIntro() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("LCD RUNNER");
  lcd.setCursor(0, 1);
  lcd.print("Jump:D7 Reset:D8");
}