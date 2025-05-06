#include "Adafruit_NeoTrellis.h"

Adafruit_NeoTrellis trellis;

#define RED   0xFF0000
#define BLUE  0x0000FF
#define WHITE 0xFFFFFF
#define OFF   0x000000
#define BRIGHTNESS 40

int board[3][3]; // 0 = empty, 1 = red, 2 = blue
int currentPlayer = 1;
bool gameOver = false;

const int gameKeys[3][3] = {
  {0, 1, 2},
  {4, 5, 6},
  {8, 9, 10}
};

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return trellis.pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
    return trellis.pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return trellis.pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void printBoard() {
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      Serial.print(board[r][c]);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println("---");
}

bool keyToBoardIndex(uint8_t key, int &row, int &col) {
  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 3; c++)
      if (gameKeys[r][c] == key) {
        row = r;
        col = c;
        return true;
      }
  return false;
}

void flashColor(uint32_t color) {
  for (int i = 0; i < 3; i++) {
    for (int r = 0; r < 3; r++)
      for (int c = 0; c < 3; c++)
        trellis.pixels.setPixelColor(gameKeys[r][c], color);
    trellis.pixels.show();
    delay(300);

    for (int r = 0; r < 3; r++)
      for (int c = 0; c < 3; c++)
        trellis.pixels.setPixelColor(gameKeys[r][c], OFF);
    trellis.pixels.show();
    delay(300);
  }
}

bool checkWin(int player) {
  for (int i = 0; i < 3; i++) {
    if (board[i][0] == player && board[i][1] == player && board[i][2] == player) return true;
    if (board[0][i] == player && board[1][i] == player && board[2][i] == player) return true;
  }
  if (board[0][0] == player && board[1][1] == player && board[2][2] == player) return true;
  if (board[0][2] == player && board[1][1] == player && board[2][0] == player) return true;
  return false;
}

bool isDraw() {
  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 3; c++)
      if (board[r][c] == 0) return false;
  return true;
}

void resetGame() {
  // Rainbow animation
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      int idx = gameKeys[r][c];
      uint32_t color = Wheel(map(idx, 0, 10, 0, 255));
      trellis.pixels.setPixelColor(idx, color);
      trellis.pixels.show();
      delay(60);
    }
  }

  delay(200);

  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      int idx = gameKeys[r][c];
      trellis.pixels.setPixelColor(idx, OFF);
      trellis.pixels.show();
      delay(60);
    }
  }

  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 3; c++)
      board[r][c] = 0;

  currentPlayer = 1;
  gameOver = false;
}

TrellisCallback onKey(keyEvent evt) {
  if (evt.bit.EDGE != SEESAW_KEYPAD_EDGE_FALLING) return 0;

  int row, col;
  if (!keyToBoardIndex(evt.bit.NUM, row, col)) return 0;

  if (gameOver) {
    resetGame();
    return 0;
  }

  if (board[row][col] != 0) return 0;

  board[row][col] = currentPlayer;
  trellis.pixels.setPixelColor(evt.bit.NUM, currentPlayer == 1 ? RED : BLUE);
  trellis.pixels.show();

  if (checkWin(currentPlayer)) {
    flashColor(currentPlayer == 1 ? RED : BLUE);
    gameOver = true;
  } 
  else if (isDraw()) {
    flashColor(WHITE);  // white blink on draw
    gameOver = true;
  } 
  else {
    currentPlayer = 3 - currentPlayer;
  }

  return 0;
}

void setup() {
  Serial.begin(9600);
  delay(100);

  if (!trellis.begin()) {
    Serial.println("Could not start Trellis!");
    while (1);
  }

  trellis.pixels.setBrightness(BRIGHTNESS);

  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 3; c++) {
      int key = gameKeys[r][c];
      trellis.activateKey(key, SEESAW_KEYPAD_EDGE_FALLING);
      trellis.registerCallback(key, onKey);
    }

  resetGame();
}

void loop() {
  trellis.read();
  delay(20);
}
