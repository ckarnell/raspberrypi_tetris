/* #include <RGBmatrixPanel.h> */
#include "led-matrix.h"
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <signal.h>

using rgb_matrix::GPIO;
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;

// LED Matrix
const int SQUARE_WIDTH = 3;
const int PIXEL_OFFSET_X = 1;
const int PIXEL_OFFSET_Y = 4;
const int SCORE_DIGITS = 8;

/* RGBMatrix::Options defaults; */
/* defaults.hardware_mapping = "adafruit-hat";  // or e.g. "regular" */
/* defaults.rows = 32; */
/* defaults.cols = 64; */
/* defaults.chain_length = 1; */
/* defaults.parallel = 1; */
/* defaults.show_refresh_rate = true; */

/* Canvas *canvas = rgb_matrix::CreateMatrixFromFlags(NULL, NULL, &defaults); */

/* RGBMatrix::Options defaults = { */
/* } */
/* RGBMatrix::Options defaults = { */
/*   "adafruit-hat", */
/*   32, // rows */
/*   64, // cols */
/*   1, // chain_length */
/*   1, // parallel */
/*   11, // pwm_bits */
/*   0, // pwm_lsb_nanoseconds */
/*   0, // pwm_dither_bits */
/*   100, // brightness */
/*   0, // scan_mode */
/*   0, // row_address_type */
/*   0, // multiplexing */
/*   show_refresh_rate = true; */
/* } */
/* Canvas *canvas = rgb_matrix::CreateMatrixFromFlags(&argc, &argv, &defaults); */

/* RGBmatrixPanel canvas(A, B, C, D, CLK, LAT, OE, false, 64); */

struct MatrixRGBColor {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

const int FOUR=146; // TODO: Why can't this be uint8_t?
const uint8_t FIVE=182;
const uint8_t TWO=73;
const uint8_t ONE=36;
const uint8_t SEVEN=255;
const uint8_t THREE=109;

const MatrixRGBColor CYAN   = { 0, FOUR, SEVEN };
const MatrixRGBColor VIOLET = { FOUR, 0, SEVEN };
const MatrixRGBColor RED    = { SEVEN, 0, 0 };
const MatrixRGBColor YELLOW = { SEVEN, SEVEN, 0 };
const MatrixRGBColor GREEN  = { 0, SEVEN, 0 };
const MatrixRGBColor ORANGE = { SEVEN, 165, 0 };
const MatrixRGBColor BLUE   = { 0, 0, SEVEN };
const MatrixRGBColor WHITE  = { ONE, ONE, ONE };
const MatrixRGBColor BLACK  = { 0, 0, 0 };
const MatrixRGBColor DIM    = { ONE, ONE, SEVEN };
const MatrixRGBColor DIM_WHITE = { TWO, TWO, TWO };
const MatrixRGBColor BRIGHT = { SEVEN, ONE, ONE };
const MatrixRGBColor LESS_BRIGHT_WHITE = { FIVE, FIVE, FIVE };

// Ghost colors
const MatrixRGBColor GHOST_J = { 0, 0, ONE };
const MatrixRGBColor GHOST_O = { ONE, ONE, 0 };
const MatrixRGBColor GHOST_Z = { ONE, 0, 0 };
const MatrixRGBColor GHOST_S = { 0, ONE, 0 };
const MatrixRGBColor GHOST_I = { 0, ONE, TWO };
const MatrixRGBColor GHOST_T = { ONE, 0, TWO };
const MatrixRGBColor GHOST_L = { TWO, ONE, 0 };

const MatrixRGBColor SETTINGS_VIOLET = { SEVEN, 0, THREE };

int adjustYCoord(int yCoord, int multiplier) {
  // We want y = 20 to appear at 60 on the led board,
  // but we want y = 40 to appear at 0 on the led board,
  // a linear function.
  // f(BUFFER_HEIGHT) = 60
  // f(24) = 0
  int result = (MAIN_MATRIX_HEIGHT+BUFFER_ZONE_HEIGHT+BORDER_SIZE)*multiplier; // Start with the max height
  result -= yCoord*multiplier;
  return result - 3;
}

void clearMatrix(Canvas* canvas) {
  /* canvas->fillRect(0, 0, canvas->width(), canvas->height(), BLACK); */
  canvas->Fill(0, 0, 0);
}

int scoreRep[SCORE_DIGITS] = {0, 0, 0, 0, 0, 0, 0, 0};

void translateScoreIntoScoreRep(long long score) {
  for (int i = 0; i < SCORE_DIGITS; i++) {
    int divideBy = pow(10, i);
    score /= divideBy;
    scoreRep[i] = score % 10;
  }
}

void newFillRect(Canvas* canvas, int startX, int startY, int width, int height, MatrixRGBColor color) {
  for (int x = startX; x < startX + width; x++) {
    for (int y = startY; y < startY + height; y++) {
      canvas->SetPixel(y, x, color.red, color.green, color.blue);
    }
  }
}

void clearBottom(Canvas* canvas) {
  /* canvas->fillRect(0, 0, 2, canvas->height(), BLACK); */
  newFillRect(canvas, 0, 0, canvas->height(), 2, BLACK);
}

void newDrawLine(Canvas* canvas, int startX, int startY, int endX, int endY, MatrixRGBColor color) {
  if (startX == endX) {
    for (int i=startY; i <= endY; i++) {
      canvas->SetPixel(i, startX, color.red, color.green, color.blue);
    }
  }

  if (startY == endY) {
    for (int i=startX; i <= endX; i++) {
      canvas->SetPixel(startY, i, color.red, color.green, color.blue);
    }
  }
}

void newDrawPixel(Canvas* canvas, int x, int y, MatrixRGBColor color) {
  canvas->SetPixel(y, x, color.red, color.green, color.blue);
}

void drawSquareNew(Canvas* canvas, int xCoord, int yCoord, MatrixRGBColor color, int multiplier, int xOffset = 0) {
//  int adjustedYCoord = adjustYCoord(yCoord, multiplier) - multiplier + PIXEL_OFFSET_Y; // Subtract 3 since the origin is seen as lower left instead of top left
  int adjustedYCoord = adjustYCoord(yCoord, multiplier) - SQUARE_WIDTH + PIXEL_OFFSET_Y; // Subtract 3 since the origin is seen as lower left instead of top left
//  if (adjustedYCoord > canvas->height()) {
//    // Nothing to draw if we're outside of the boord
//    return;
//  }
  int adjustedXCoord = ((xCoord - 1) * multiplier) + PIXEL_OFFSET_X + xOffset;
  /* canvas->fillRect(adjustedYCoord, adjustedXCoord, multiplier, multiplier, color); */
  newFillRect(canvas, adjustedXCoord, adjustedYCoord, multiplier, multiplier, color);
}


void clearNextPieces(Canvas* canvas) {
  newFillRect(canvas, 6, 0, canvas->height(), 3, BLACK);
}

void clearHeldPiece(Canvas* canvas) {
  newFillRect(canvas, 0, 0, 5, 3, BLACK);
}



void drawDigit(Canvas* canvas, int digit, int startingX, int startingY, MatrixRGBColor color) {
  // Font from here: https://fontstruct.com/fontstructions/show/1422505/5x4-pxl
  // Starting X is from the left and starting Y is from the bottom
  switch (digit) {
    case 0:
      //0000
      //00 0
      //00 0
      //00 0
      //0000

      /* canvas->drawLine(startingY, startingX, startingY, startingX+3, color); */
      newDrawLine(canvas, startingX, startingY, startingX+3, startingY, color);
      /* canvas->drawLine(startingY+4, startingX, startingY+4, startingX+3, color); */
      newDrawLine(canvas, startingX, startingY+4, startingX+3, startingY+4, color);

      /* canvas->drawLine(startingY+1, startingX, startingY+3, startingX, color); */
      newDrawLine(canvas, startingX, startingY+1, startingX, startingY+3, color);

      /* canvas->drawLine(startingY+1, startingX+1, startingY+3, startingX+1, color); */
      newDrawLine(canvas, startingX+1, startingY+1, startingX+1, startingY+3, color);

      /* canvas->drawLine(startingY+1, startingX+3, startingY+3, startingX+3, color); */
      newDrawLine(canvas, startingX+3, startingY+1, startingX+3, startingY+3, color);

      break;
    case 1:
      // 111
      //1111
      // 111
      // 111
      // 111
      // 111

      newDrawPixel(canvas, startingX, startingY+3, color);
      newFillRect(canvas, startingX + 1, startingY, 3, 5, color);

      break;
    case 2:
      //2222
      //  22
      //2222
      //22
      //2222

      newDrawLine(canvas, startingX, startingY, startingX+3, startingY, color);
      newDrawLine(canvas, startingX, startingY+1, startingX+1, startingY+1, color);
      newDrawLine(canvas, startingX, startingY+2, startingX+3, startingY+2, color);
      newDrawLine(canvas, startingX+2, startingY+3, startingX+3, startingY+3, color);
      newDrawLine(canvas, startingX, startingY+4, startingX+3, startingY+4, color);

      break;
    case 3:
      //3333
      //  33
      // 333
      //  33
      //3333

      newDrawLine(canvas, startingX, startingY, startingX+3, startingY, color);
      newDrawLine(canvas, startingX, startingY+4, startingX+3, startingY+4, color);
      newFillRect(canvas, startingX+2, startingY+1, 2, 3, color);
      newDrawPixel(canvas, startingX+1, startingY+2, color);
      break;
    case 4:
      //4 44
      //4 44
      //4444
      //  44
      //  44

      newFillRect(canvas, startingX+2, startingY, 2, 5, color);
      newDrawLine(canvas, startingX, startingY+2, startingX, startingY+4, color);
      newDrawPixel(canvas, startingX+1, startingY+2, color);
      break;
    case 5:
      //5555
      //55
      //5555
      //  55
      //5555

      newFillRect(canvas, startingX, startingY+2, 2, 3, color);
      newFillRect(canvas, startingX+2, startingY, 2, 3, color);
      newDrawLine(canvas, startingX, startingY, startingX+1, startingY, color);
      newDrawLine(canvas, startingX+2, startingY+4, startingX+3, startingY+4, color);
      break;
    case 6:
      //6666
      //66
      //6666
      //66 6
      //6666

      newFillRect(canvas, startingX, startingY, 2, 5, color);
      newDrawLine(canvas, startingX+2, startingY, startingX+3, startingY, color);
      newDrawLine(canvas, startingX+2, startingY+2, startingX+3, startingY+2, color);
      newDrawLine(canvas, startingX+2, startingY+4, startingX+3, startingY+4, color);
      newDrawPixel(canvas, startingX+3, startingY+1, color);
      break;
    case 7:
      //7777
      //  77
      //  77
      //  77
      //  77

      newFillRect(canvas, startingX+2, startingY, 2, 5, color);
      newDrawLine(canvas, startingX, startingY+4, startingX+3, startingY+4, color);
      break;
    case 8:
      //8888
      //88 8
      //8888
      //88 8
      //8888

      newFillRect(canvas, startingX, startingY, 2, 5, color);
      newDrawLine(canvas, startingX+2, startingY, startingX+3, startingY, color);
      newDrawPixel(canvas, startingX+3, startingY+1, color);
      newDrawLine(canvas, startingX+2, startingY+2, startingX+3, startingY+2, color);
      newDrawPixel(canvas, startingX+3, startingY+3, color);
      newDrawLine(canvas, startingX+2, startingY+4, startingX+3, startingY+4, color);
      break;
    case 9:
      //9999
      //9 99
      //9999
      //  99
      //9999

      newFillRect(canvas, startingX+2, startingY, 2, 5, color);
      newDrawLine(canvas, startingX, startingY, startingX+1, startingY, color);
      newDrawLine(canvas, startingX, startingY+2, startingX+1, startingY+2, color);
      newDrawPixel(canvas, startingX, startingY+3, color);
      newDrawLine(canvas, startingX, startingY+4, startingX+1, startingY+4, color);
      break;
  }
}

const int LETTER_WIDTH = 4;

void drawLetter(Canvas* canvas, char letter, int startingX, int startingY, MatrixRGBColor color) {
  switch (letter) {
    case 'A':
      //AAAA
      //A  A
      //AAAA
      //A  A
      //A  A

      newDrawLine(canvas, startingX, startingY, startingX, startingY+4, color);
      newDrawLine(canvas, startingX+3, startingY, startingX+3, startingY+4, color);
      newDrawLine(canvas, startingX+1, startingY+2, startingX+2, startingY+2, color);
      newDrawLine(canvas, startingX+1, startingY+4, startingX+2, startingY+4, color);
      break;
    case 'C':
      //CCCC
      //C
      //C
      //C
      //CCCC

      newDrawLine(canvas, startingX, startingY, startingX+3, startingY, color);
      newDrawLine(canvas, startingX, startingY+1, startingX, startingY+3, color);
      newDrawLine(canvas, startingX, startingY+4, startingX+3, startingY+4, color);
      break;
    case 'E':
      //EEEE
      //E
      //EEEE
      //E
      //EEEE

      newDrawLine(canvas, startingX, startingY, startingX+3, startingY, color);
      newDrawLine(canvas, startingX, startingY+2, startingX+3, startingY+2, color);
      newDrawLine(canvas, startingX, startingY+4, startingX+3, startingY+4, color);
      newDrawPixel(canvas, startingX, startingY+1, color);
      newDrawPixel(canvas, startingX, startingY+3, color);
      break;
    case 'G':
      //GGGG
      //G
      //G GG
      //G  G
      //GGGG

      newDrawLine(canvas, startingX, startingY, startingX+3, startingY, color);
      newDrawLine(canvas, startingX, startingY+1, startingX, startingY+3, color);
      newDrawLine(canvas, startingX+3, startingY+1, startingX+3, startingY+2, color);
      newDrawPixel(canvas, startingX+2, startingY+2, color);
      newDrawLine(canvas, startingX, startingY+4, startingX+3, startingY+4, color);
      break;
    case 'H':
      //H  H
      //H  H
      //HHHH
      //H  H
      //H  H

      newDrawLine(canvas, startingX, startingY, startingX, startingY+4, color);
      newDrawLine(canvas, startingX+3, startingY, startingX+3, startingY+4, color);
      newDrawLine(canvas, startingX+1, startingY+2, startingX+2, startingY+2, color);
      break;
    case 'I':
      //IIII
      // II 
      // II 
      // II 
      //IIII

      newDrawLine(canvas, startingX, startingY, startingX+3, startingY, color);
      newDrawLine(canvas, startingX, startingY+4, startingX+3, startingY+4, color);
      newFillRect(canvas, startingX+1, startingY+1, 2, 3, color);
      break;
    case 'M':
      //MMMM
      //MMMM
      //M  M
      //M  M
      //M  M

      newFillRect(canvas, startingX, startingY+3, 4, 2, color);
      newDrawLine(canvas, startingX, startingY, startingX, startingY+2, color);
      newDrawLine(canvas, startingX+3, startingY, startingX+3, startingY+2, color);
      break;
    case 'O':
      //OOOO
      //O  O
      //O  O
      //O  O
      //OOOO

      newDrawLine(canvas, startingX, startingY, startingX+3, startingY, color);
      newDrawLine(canvas, startingX, startingY+4, startingX+3, startingY+4, color);
      newDrawLine(canvas, startingX, startingY+1, startingX, startingY+3, color);
      newDrawLine(canvas, startingX+3, startingY+1, startingX+3, startingY+3, color);
      break;
    case 'R':
      //RRR
      //R  R
      //RRR
      //R  R
      //R  R

      newDrawLine(canvas, startingX, startingY, startingX, startingY+4, color);
      newDrawLine(canvas, startingX+1, startingY+2, startingX+2, startingY+2, color);
      newDrawLine(canvas, startingX+1, startingY+4, startingX+2, startingY+4, color);
      newDrawLine(canvas, startingX+3, startingY, startingX+3, startingY+1, color);
      newDrawPixel(canvas, startingX+3, startingY+3, color);
      break;
    case 'S':
      //SSSS
      //S
      //SSSS
      //   S
      //SSSS

      newDrawLine(canvas, startingX, startingY, startingX+3, startingY, color);
      newDrawLine(canvas, startingX, startingY+2, startingX+3, startingY+2, color);
      newDrawLine(canvas, startingX, startingY+4, startingX+3, startingY+4, color);
      newDrawPixel(canvas, startingX+3, startingY+1, color);
      newDrawPixel(canvas, startingX, startingY+3, color);
      break;
    case 'V':
      //V  V
      //V  V
      //V  V
      //V V
      // V

      newDrawLine(canvas, startingX, startingY+1, startingX, startingY+4, color);
      newDrawPixel(canvas, startingX+1, startingY, color);
      newDrawPixel(canvas, startingX+2, startingY+1, color);
      newDrawLine(canvas, startingX+3, startingY+2, startingX+3, startingY+4, color);
      break;
  }
}

void drawGameOver(Canvas* canvas, int y) {
  drawLetter(canvas, 'G', 0,              y, CYAN);
  drawLetter(canvas, 'A', LETTER_WIDTH,   y, VIOLET);
  drawLetter(canvas, 'M', LETTER_WIDTH*2, y, RED);
  drawLetter(canvas, 'E', LETTER_WIDTH*3, y, YELLOW);
  drawLetter(canvas, 'O', LETTER_WIDTH*4, y, GREEN);
  drawLetter(canvas, 'V', LETTER_WIDTH*5, y, ORANGE);
  drawLetter(canvas, 'E', LETTER_WIDTH*6, y, BLUE);
  drawLetter(canvas, 'R', LETTER_WIDTH*7, y, CYAN);
}

void drawScore(Canvas* canvas, int y) {
  drawLetter(canvas, 'S', 0, y, LESS_BRIGHT_WHITE);
  drawLetter(canvas, 'C', LETTER_WIDTH + 1, y, LESS_BRIGHT_WHITE);
  drawLetter(canvas, 'O', LETTER_WIDTH*2 + 2, y, LESS_BRIGHT_WHITE);
  drawLetter(canvas, 'R', LETTER_WIDTH*3 + 3, y, LESS_BRIGHT_WHITE);
  drawLetter(canvas, 'E', LETTER_WIDTH*4 + 4, y, LESS_BRIGHT_WHITE);
}

void drawHigh(Canvas* canvas, int y) {
  drawLetter(canvas, 'H', 0, y, LESS_BRIGHT_WHITE);
  drawLetter(canvas, 'I', LETTER_WIDTH + 1, y, LESS_BRIGHT_WHITE);
  drawLetter(canvas, 'G', LETTER_WIDTH*2 + 2, y, LESS_BRIGHT_WHITE);
  drawLetter(canvas, 'H', LETTER_WIDTH*3 + 3, y, LESS_BRIGHT_WHITE);
}

void drawNumber(Canvas* canvas, long long* originalNum, int y) {
  long long num = *originalNum;
  int startX = canvas->height() - 4;
  MatrixRGBColor color = BRIGHT;
  int numToDraw;

  while (startX >= 0) {
    numToDraw = num % 10;
    color = color.red == ONE ? BRIGHT : DIM;
    drawDigit(canvas, numToDraw, startX, y, color);
    num /= 10;
    startX -= 4;
  }
}
