// **** SCREEN ****
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <SPI.h>

#define TFT_CS 10
#define TFT_RST -1
#define TFT_DC 8

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// **** Joystick ****
#define joystickX_pin A0
#define josytickY_pin A1
#define joystickButton_pin 2

int joystickX;
int joystickY;
int josytickButton;

// *** IR Remote ***
#include <IRremote.h>  // V2.8.0

#define irUpVal 16718055 // this is 2 on my remote
#define irRightVal 16734885 // 6 on my remote
#define irDownVal 16730805 // 8
#define irLeftVal 16716015 // 4

const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results irResults;
uint32_t irValidCode;

// *** Snake  ***
#define snakeMaxSize 100
#define snakeMinSize 1

typedef struct Snake;
struct Snake {
  bool alive;
  int head[2];
  int body[snakeMaxSize][2];
  int currentLength;
  int direction[2];
  int prevDirection[2];  // Used to allow the player to change direction between "frames" / updates, making the game feel more responsive
};

typedef struct Fruit;
struct Fruit {
  int xCoord;
  int yCoord;
};

int cellSize = 10;
int gridX = 240 / cellSize;
int gridY = 135 / cellSize;

int playAreaMaxX = gridY - 1;
int playAreaMinX = 1;
int playAreaMaxY = gridY - 1;
int playAreaMinY = 1;

int scoreTextCursorX = playAreaMaxX * cellSize + cellSize * 3;
int scoreTextCursorY = cellSize;
int currentScore = 0;
int prevScore = 0;

int highScoreTextCursorX = 0;
int highScoreTextCursorY = 0;
int highScore = 0;

int ySpawn = (gridY / 2);
Snake snake = { true, { 1, ySpawn }, { { 1, ySpawn } }, snakeMinSize, { 1, 0 }, { 1, 0 } };  //Initialize snake object
Fruit fruit = { playAreaMaxX / 2, playAreaMaxY / 2 };

bool directionLock = true;
long currentMillis = 0;
long lastUpdate = 0;
int updateIntervalMs = 150;

void setup() {
  Serial.begin(9600);

  // Joystick setup
  pinMode(joystickButton_pin, INPUT);
  digitalWrite(joystickButton_pin, HIGH);

  // Screen setup
  tft.init(135, 240);
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(1);

  // IR remote setup
  irrecv.enableIRIn();
  irrecv.blink13(true);

  // Snake game setup
  setupGameAreaBorder();
  setupScoreAndHighscore();
}


void loop() {
  currentMillis = millis();
  joystickX = analogRead(joystickX_pin);
  joystickY = analogRead(josytickY_pin);
  // josytickButton = digitalRead(joystickButton_pin); Not really used

  if (irrecv.decode(&irResults)) {
    uint32_t val = irResults.value;
    // Check if the value given is one of our "valid" move values, no point saving if it isn't.
    if (val == irUpVal || val == irRightVal || val == irDownVal || val == irLeftVal) {
      irValidCode = val;
    }
    irrecv.resume();
  }

  if ((joystickX < 100 && snake.direction[0] == 0 || irValidCode == irLeftVal) && snake.prevDirection[0] != 1) {
    // Go left
    snake.direction[0] = -1;
    snake.direction[1] = 0;
  } else if ((joystickX > 920 && snake.direction[0] == 0 || irValidCode == irRightVal) && snake.prevDirection[0] != -1) {
    // Go right
    snake.direction[0] = 1;
    snake.direction[1] = 0;
  } else if ((joystickY < 100 && snake.direction[1] == 0 || irValidCode == irUpVal) && snake.prevDirection[1] != 1) {
    // Go "up" (it's screen coordinate system, so 0,0 starts in the upper left)
    snake.direction[0] = 0;
    snake.direction[1] = -1;
  } else if ((joystickY > 920 && snake.direction[1] == 0 || irValidCode == irDownVal) && snake.prevDirection[1] != -1) {
    // Go down
    snake.direction[0] = 0;
    snake.direction[1] = 1;
  }

  if (currentMillis - lastUpdate > updateIntervalMs) {
    lastUpdate = currentMillis;
    irValidCode = 0; // reset the IR code
    if (snake.alive == true) {
      snake.prevDirection[0] = snake.direction[0];
      snake.prevDirection[1] = snake.direction[1];

      snake.head[0] += snake.direction[0];
      snake.head[1] += snake.direction[1];

      // Looping borders
      if (snake.head[0] < playAreaMinX) {
        snake.head[0] = playAreaMaxX;
      } else if (snake.head[0] > playAreaMaxX) {
        snake.head[0] = playAreaMinX;
      } else if (snake.head[1] < playAreaMinY) {
        snake.head[1] = playAreaMaxY;
      } else if (snake.head[1] > playAreaMaxY) {
        snake.head[1] = playAreaMinY;
      }

      // check if snake eats fruit
      if (snake.head[0] == fruit.xCoord && snake.head[1] == fruit.yCoord) {
        snake.currentLength++;
        currentScore++;

        // "Spawn" new fruit
        /** TODO: Prevent fruit from spawning inside the snake */
        fruit.xCoord = (int)random(playAreaMinX + 1, playAreaMaxX - 1);
        fruit.yCoord = (int)random(playAreaMinY + 1, playAreaMaxY - 1);

        writeScore(currentScore, prevScore);
        prevScore = currentScore;

        if (highScore < currentScore) {
          writeHighscore(currentScore, highScore);
          highScore = currentScore;
        }
      } else {
        // Left shift snake.body array, essentially making every segment follow the head
        for (int i = 0; i < snake.currentLength; i++) {
          snake.body[i][0] = snake.body[i + 1][0];
          snake.body[i][1] = snake.body[i + 1][1];
        }
      }

      // Add the head to the snake body
      snake.body[snake.currentLength][0] = snake.head[0];
      snake.body[snake.currentLength][1] = snake.head[1];

      // check if snake eats itself (cannot eat itself with less than 3 segments)
      for (int i = 1; i < snake.currentLength - 3; i++) {
        if (snake.head[0] == snake.body[i][0] && snake.head[1] == snake.body[i][1]) {
          snake.alive = false;
        }
      }

      // remove the old tail from the screen
      tft.drawRect(snake.body[0][0] * cellSize, snake.body[0][1] * cellSize, cellSize, cellSize, ST77XX_BLACK);
      tft.fillRect(snake.body[0][0] * cellSize, snake.body[0][1] * cellSize, cellSize, cellSize, ST77XX_BLACK);

      // draw the head
      tft.drawRect(snake.head[0] * cellSize, snake.head[1] * cellSize, cellSize, cellSize, ST77XX_RED);
      tft.fillRect(snake.head[0] * cellSize, snake.head[1] * cellSize, cellSize, cellSize, ST77XX_RED);

      // draw the "neck"
      if (1 < snake.currentLength) {
        tft.fillRect(snake.body[snake.currentLength - 1][0] * cellSize, snake.body[snake.currentLength - 1][1] * cellSize, cellSize, cellSize, ST77XX_BLACK);
        tft.drawRect(snake.body[snake.currentLength - 1][0] * cellSize, snake.body[snake.currentLength - 1][1] * cellSize, cellSize, cellSize, ST77XX_RED);
      }

      // draw the fruit
      tft.drawRect(fruit.xCoord * cellSize, fruit.yCoord * cellSize, cellSize, cellSize, ST77XX_GREEN);

    } else if (snake.alive == false) {
      tft.fillRect(snake.body[snake.currentLength - 1][0] * cellSize, snake.body[snake.currentLength - 1][1] * cellSize, cellSize, cellSize, ST77XX_BLACK);
      snake.currentLength--;

      if (snake.currentLength < 0) {
        prevScore = currentScore;
        currentScore = 0;

        snake.head[0] = 0;
        snake.head[1] = ySpawn;
        snake.currentLength = snakeMinSize;
        for (int i = 0; i < snake.currentLength + 1; i++) {
          snake.body[i][0] = -1;
          snake.body[i][1] = -1;
        }

        snake.alive = true;
      }
    }
  }
}

void setupGameAreaBorder() {
  // Horizontal game area lines
  tft.drawLine(playAreaMinX * cellSize - 1, playAreaMinY + cellSize - 2, playAreaMaxX * cellSize + cellSize + 1, playAreaMinY + cellSize - 2, ST77XX_BLUE);
  tft.drawLine(playAreaMinX * cellSize - 1, playAreaMaxY * cellSize + cellSize + 1, playAreaMaxX * cellSize + cellSize + 1, playAreaMaxY * cellSize + cellSize + 1, ST77XX_BLUE);

  // Vertical game area lines
  tft.drawLine(playAreaMinX + cellSize - 2, playAreaMinY * cellSize, playAreaMinX + cellSize - 2, (playAreaMaxY * cellSize) + cellSize + 1, ST77XX_BLUE);
  tft.drawLine((playAreaMaxX * cellSize) + cellSize + 1, playAreaMinY * cellSize, (playAreaMaxX * cellSize) + cellSize + 1, (playAreaMaxY * cellSize) + cellSize + 1, ST77XX_BLUE);
}

void setupScoreAndHighscore() {
  tft.setCursor(scoreTextCursorX, scoreTextCursorY);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_BLUE);
  tft.print("Score:");

  tft.println();
  tft.println();
  tft.println();

  tft.setCursor(scoreTextCursorX, tft.getCursorY());
  tft.println("High");
  tft.setCursor(scoreTextCursorX, tft.getCursorY());
  highScoreTextCursorX = tft.getCursorX();
  tft.print("score:");

  highScoreTextCursorY = tft.getCursorY();
}

void writeScore(int newScore, int prevScore) {
  // remove the prev score
  tft.setCursor(scoreTextCursorX, scoreTextCursorY);
  tft.setTextColor(ST77XX_BLACK);
  tft.println();
  tft.setCursor(scoreTextCursorX, tft.getCursorY() + 3);
  tft.print(" ");
  tft.print(prevScore);

  // write the new score
  tft.setCursor(scoreTextCursorX, scoreTextCursorY);
  tft.setTextColor(ST77XX_BLUE);
  tft.println();
  tft.setCursor(scoreTextCursorX, tft.getCursorY() + 3);
  tft.print(" ");
  tft.print(newScore);
}

void writeHighscore(int newHighScore, int prevHighScore) {
  tft.setCursor(highScoreTextCursorX, highScoreTextCursorY);
  tft.setCursor(highScoreTextCursorX, highScoreTextCursorY);
  tft.setTextColor(ST77XX_BLACK);
  tft.println();
  tft.setCursor(highScoreTextCursorX, tft.getCursorY() + 3);
  tft.print(" ");
  tft.print(prevHighScore);

  tft.setCursor(highScoreTextCursorX, highScoreTextCursorY);
  tft.setCursor(highScoreTextCursorX, highScoreTextCursorY);
  tft.setTextColor(ST77XX_BLUE);
  tft.println();
  tft.setCursor(highScoreTextCursorX, tft.getCursorY() + 3);
  tft.print(" ");
  tft.print(newHighScore);
}

void debugJoystick() {
  Serial.print("x: ");
  Serial.print(joystickX);

  Serial.print(" y: ");
  Serial.print(joystickY);

  Serial.print(" click: ");
  Serial.print(josytickButton);

  Serial.println();
}