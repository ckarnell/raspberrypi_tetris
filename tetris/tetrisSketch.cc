// Compile this directory with `make -C tetris`
// Run with `./tetris/tetrisSketch --led-slowdown-gpio=4`
/* #include "led-matrix.h" */
#include "engine.h"
#include "draw.h"

/* #include <unistd.h> */
/* #include <math.h> */
/* #include <stdio.h> */
#include <iostream>
/* #include <SFML/Graphics.hpp> */
/* #include <SFML/Window.hpp> */
#include <SFML/Window/Joystick.hpp>
/* #include <signal.h> */

/* using rgb_matrix::GPIO; */
/* using rgb_matrix::RGBMatrix; */
/* using rgb_matrix::Canvas; */

TetrisEngine tetrisEngine = TetrisEngine();

const MatrixRGBColor BORDER_PLACEHOLDER = { 1, 1, 1 };
const MatrixRGBColor colorMap[10] = {BLACK, BORDER_PLACEHOLDER, VIOLET, GREEN, RED, CYAN, ORANGE, BLUE, YELLOW, WHITE};
const int MATRIX_ROWS = 32;
const int MATRIX_COLS = 64;

bool gameOver = false;
bool gameOverDrawn = false;

bool firstIteration = true;
long gameOverAt;
long timeBeforeSleep = 60000;

int ghostInds[4][2] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

struct Controls controls;
int newGamePushed = false;
int newGameReleased = false;

int ghostSettingsPushed = false;
int ghostSettingsReleased = false;

long long highScore = 0;
bool shouldDrawGhost = true;
bool shouldDrawPiece = true;

void drawGhost(Canvas* canvas) {
  // Undraw the last ghost
  if (!shouldDrawGhost) {
    return;
  }

  if (tetrisEngine.rowsRemovedThisIteration != 0) {
    for (int i = 0; i < 4; i++) {
      drawSquareNew(canvas, ghostInds[i][0], ghostInds[i][1], BLACK, 3);
      ghostInds[i][0] = -1;
      ghostInds[i][1] = -1;
    }
    return;
  }

  MatrixRGBColor ghostColor = WHITE;
  switch (tetrisEngine.currentPiece -> symbol) {
    case 'J':
      ghostColor = GHOST_J;
      break;
    case 'O':
      ghostColor = GHOST_O;
      break;
    case 'Z':
      ghostColor = GHOST_Z;
      break;
    case 'S':
      ghostColor = GHOST_S;
      break;
    case 'I':
      ghostColor = GHOST_I;
      break;
    case 'T':
      ghostColor = GHOST_T;
      break;
    case 'L':
      ghostColor = GHOST_L;
      break;
  }

  int newGhostInds[4][2] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};

  // Draw the ghost
  int currentGhostInd = 0;
  int currentGhostY = tetrisEngine.getYModifierAfterHardDrop() + tetrisEngine.currentY;
  for (int y = 0; y < tetrisEngine.currentPiece -> dimension; y++) {
    for (int x = 0; x < tetrisEngine.currentPiece -> dimension; x++) {
      int minoRepresentation = tetrisEngine.currentPiece -> orientations[tetrisEngine.orientation][y][x];
      if (minoRepresentation == 1) {
        if (tetrisEngine.matrixRepresentation[(currentGhostY + y)*tetrisEngine.fieldWidth + (x + tetrisEngine.currentX)] == 0) {
          drawSquareNew(canvas, x + tetrisEngine.currentX, currentGhostY + y, ghostColor, 3);
          newGhostInds[currentGhostInd][0] = x + tetrisEngine.currentX;
          newGhostInds[currentGhostInd][1] = currentGhostY + y;
        } else {
          newGhostInds[currentGhostInd][0] = -1;
          newGhostInds[currentGhostInd][1] = -1;
        }
        currentGhostInd++;
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    bool shouldDelete = true;
    for (int j = 0; j < 4; j++) {
      if (ghostInds[i][0] == newGhostInds[j][0] && ghostInds[i][1] == newGhostInds[j][1]) {
        shouldDelete = false;
      }
    }

    if (ghostInds[i][0] != -1 && shouldDelete) {
       if (tetrisEngine.matrixRepresentation[ghostInds[i][1]*tetrisEngine.fieldWidth + ghostInds[i][0]] == 0) {
         drawSquareNew(canvas, ghostInds[i][0], ghostInds[i][1], BLACK, 3);
       }
       ghostInds[i][0] = -1;
       ghostInds[i][1] = -1;
    }
  }


  // Copy over
  for (int i = 0; i < 4; i++) {
    ghostInds[i][0] = newGhostInds[i][0];
    ghostInds[i][1] = newGhostInds[i][1];
  }
}



// Print next pieces
void printNextPieces(Canvas* canvas) {
  drawGhost(canvas);
  clearNextPieces(canvas);
  for (int i = 0; i < 5; i++) {
    Tetromino* nextPiece = tetrisEngine.bag.getFuturePiece(i + 1);

    for (int y = 0; y < nextPiece -> dimension; y++) {
      for (int x = 0; x < nextPiece -> dimension; x++) {
        if (nextPiece -> orientations[0][y][x] == 1) {
          int adjustedX = x + canvas->height() - 5 - 5*i;
          newDrawPixel(canvas, adjustedX, 1-y, colorMap[nextPiece -> symbolNum]);
          /* int xOffset = 5; */
          /* if (nextPiece -> symbolNum == 8) */
          /*   xOffset = -1; */
          /* drawSquareNew(canvas, x + 13 - 4*i, y + tetrisEngine.fieldHeight - 1, colorMap[nextPiece -> symbolNum], 1, xOffset); */
        /* } */
        }
      }
    }
  }
}

void printWholeBoard(Canvas* canvas) {
  for(int y = BUFFER_ZONE_HEIGHT; y < tetrisEngine.fieldHeight; y++) {
    for(int x = 0; x < tetrisEngine.fieldWidth; x++) {
      int currentNum = tetrisEngine.matrixRepresentation[y*tetrisEngine.fieldWidth + x];


      int currentColorInd = currentNum;
      if (currentNum == CURRENT_PIECE_CHAR) {
        if (shouldDrawPiece || tetrisEngine.gameController.dropPressed || tetrisEngine.firstPiece) {
          currentColorInd = tetrisEngine.currentPiece -> symbolNum;
        } else {
          // Draw black if we don't want to draw the current piece
          currentColorInd = 0;
        }
      }
      /* int currentColorInd = currentNum == CURRENT_PIECE_CHAR ? tetrisEngine.currentPiece -> symbolNum : currentNum; */
      
      if (currentColorInd != 1) { // Don't draw borders
        MatrixRGBColor currentColor = colorMap[currentColorInd];
        drawSquareNew(canvas, x, y, currentColor, 3);
      }
    }
  }
}

int main(int argc, char *argv[]) {
  /* bool pressed = sf::Joystick::isButtonPressed(0, 2); */
  /* std::cout << "Pressed: " << pressed << std::endl; */

  RGBMatrix::Options defaults;
  defaults.hardware_mapping = "adafruit-hat";  // or e.g. "regular"
  /* defaults.gpio_slowdown = 3; */
  defaults.rows = 32;
  defaults.cols = 64;
  defaults.chain_length = 1;
  defaults.parallel = 1;
  defaults.show_refresh_rate = false;
  Canvas *canvas = rgb_matrix::CreateMatrixFromFlags(&argc, &argv, &defaults);

  if (canvas == NULL)
    return 1;

  // It is always good to set up a signal handler to cleanly exit when we
  // receive a CTRL-C for instance. The DrawOnCanvas() routine is looking
  // for that.
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  /* std::cout << "Connected 0: " << sf::Joystick::isConnected(0) << std::endl; */
  /* std::cout << "Connected 1: " << sf::Joystick::isConnected(1) << std::endl; */
  /* std::cout << "Connected 2: " << sf::Joystick::isConnected(2) << std::endl; */
  /* std::cout << "Connected 3: " << sf::Joystick::isConnected(3) << std::endl; */
  /* std::cout << "Connected 4: " << sf::Joystick::isConnected(4) << std::endl; */
  sf::Joystick::update();
  std::cout << "Connected 0: " << sf::Joystick::isConnected(0) << std::endl;
  /* std::cout << "Connected 1: " << sf::Joystick::isConnected(1) << std::endl; */
  /* std::cout << "Connected 2: " << sf::Joystick::isConnected(2) << std::endl; */
  /* std::cout << "Connected 3: " << sf::Joystick::isConnected(3) << std::endl; */

  while (true) {
    int squareIsPressed = sf::Joystick::isButtonPressed(0, 0);
    int crossIsPressed = sf::Joystick::isButtonPressed(0, 1);
    int circleIsPressed = sf::Joystick::isButtonPressed(0, 2);
    int triangleIsPressed = sf::Joystick::isButtonPressed(0, 3);
    int l1IsPressed = sf::Joystick::isButtonPressed(0, 4);
    int r1IsPressed = sf::Joystick::isButtonPressed(0, 5);
    int l2IsPressed = sf::Joystick::isButtonPressed(0, 6);
    int r2IsPressed = sf::Joystick::isButtonPressed(0, 7);
    /* int shareIsPressed = sf::Joystick::isButtonPressed(0, 8); */
    int optionsIsPressed = sf::Joystick::isButtonPressed(0, 9);
    int psButtonIsPressed = sf::Joystick::isButtonPressed(0, 12);
    int povX = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX);
    int povY = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY);

    /* std::cout << "Button 0: " << sf::Joystick::isButtonPressed(0, 0) << std::endl; // SQUARE */
    /* std::cout << "Button 1: " << sf::Joystick::isButtonPressed(0, 1) << std::endl; // CROSS */
    /* std::cout << "Button 2: " << sf::Joystick::isButtonPressed(0, 2) << std::endl; // CIRCLE */
    /* std::cout << "Button 3: " << sf::Joystick::isButtonPressed(0, 3) << std::endl; // TRIANGLE */
    /* std::cout << "Button 4: " << sf::Joystick::isButtonPressed(0, 4) << std::endl; // L1 */
    /* std::cout << "Button 5: " << sf::Joystick::isButtonPressed(0, 5) << std::endl; // R1 */
    /* std::cout << "Button 6: " << sf::Joystick::isButtonPressed(0, 6) << std::endl; // L2 */
    /* std::cout << "Button 7: " << sf::Joystick::isButtonPressed(0, 7) << std::endl; // R2 */
    /* std::cout << "Button 8: " << sf::Joystick::isButtonPressed(0, 8) << std::endl; // SHARE */
    /* std::cout << "Button 9: " << sf::Joystick::isButtonPressed(0, 9) << std::endl; // OPTIONS */
    /* std::cout << "Button 10: " << sf::Joystick::isButtonPressed(0, 10) << std::endl; */
    /* std::cout << "Button 11: " << sf::Joystick::isButtonPressed(0, 11) << std::endl; */
    /* std::cout << "Button 12: " << sf::Joystick::isButtonPressed(0, 12) << std::endl; // PS BUTTON */
    /* std::cout << "Button 13: " << sf::Joystick::isButtonPressed(0, 13) << std::endl; */
    /* std::cout << "Button 14: " << sf::Joystick::isButtonPressed(0, 14) << std::endl; */
    /* std::cout << "Button 15: " << sf::Joystick::isButtonPressed(0, 15) << std::endl; */
    /* std::cout << "Button 16: " << sf::Joystick::isButtonPressed(0, 16) << std::endl; */
    /* std::cout << "X AXIS: " << sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) << std::endl; */
    /* std::cout << "Y AXIS: " << sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y) << std::endl; */
    /* std::cout << "Z AXIS: " << sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Z) << std::endl; */
    /* std::cout << "U AXIS: " << sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U) << std::endl; */
    /* std::cout << "V AXIS: " << sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::V) << std::endl; */
    /* std::cout << "PovX AXIS: " << sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX) << std::endl; */
    /* std::cout << "PovY AXIS: " << sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY) << std::endl; */
    sf::Joystick::update();

    if (interrupt_received)
      return 1;

    int upValue = povY == -100;
    int leftValue = povX == -100;
    int rightValue = povX == 100;
    int downValue = povY == 100;
    int holdValue = r1IsPressed + l1IsPressed;
    int flipValue = r2IsPressed + l2IsPressed;
    int startValue = psButtonIsPressed;
    int selectValue = optionsIsPressed;
    int clockwiseButtonValue = triangleIsPressed + circleIsPressed;
    int counterClockwiseButtonValue = squareIsPressed + crossIsPressed;

    if (gameOver && !ghostSettingsPushed && flipValue == 1) {
      ghostSettingsPushed = true;
    }

    if (gameOver && ghostSettingsPushed && flipValue == 0) {
      ghostSettingsPushed = false;
      ghostSettingsReleased = true;
    }

    if (gameOver && !newGamePushed && counterClockwiseButtonValue == 1) {
      newGamePushed = true;
    }

    if (gameOver && newGamePushed && counterClockwiseButtonValue == 0) {
      newGameReleased = true;
      newGamePushed = false;
    }

    controls = {
      rightValue, // Right
      leftValue, // Left
      upValue, // Up
      downValue, // Down
      clockwiseButtonValue == 1, // Clockwise
      counterClockwiseButtonValue == 1, // Counter clockwise
      flipValue, // Rotate 180
      holdValue, // Hold
      selectValue, // Select
      startValue, // Start
    };

    if (gameOver && !gameOverDrawn) {
      highScore = tetrisEngine.score > highScore ? tetrisEngine.score : highScore;
      gameOverDrawn = true;

      int wordHeightOffset = 6; // Height of font + 1 space + 1 to set next draw location
      int currentY = 18;
      int lineOffset = 1;
      int highestY = currentY + lineOffset*4 + wordHeightOffset*5;

      newFillRect(canvas, 0, currentY, canvas->height(), highestY - currentY, BLACK);
      newDrawLine(canvas, 0, currentY - 1, canvas->height() - 1, currentY - 1, DIM_WHITE);
      newDrawLine(canvas, 0, highestY-1, canvas->height() - 1, highestY-1, DIM_WHITE);

      currentY += lineOffset;

      drawNumber(canvas, &highScore, currentY);
      currentY += wordHeightOffset;

      drawHigh(canvas, currentY);
      currentY += wordHeightOffset + lineOffset;

      drawNumber(canvas, &tetrisEngine.score, currentY);
      currentY += wordHeightOffset;

      drawScore(canvas, currentY);
      currentY += wordHeightOffset + lineOffset;


      drawGameOver(canvas, currentY);

      if (!shouldDrawGhost) {
        newDrawPixel(canvas, canvas->height()-1, canvas->width()-1, SETTINGS_VIOLET);
      }
    }

    if (gameOver) {
      firstIteration = true;
      if (newGameReleased) {
        newGameReleased = false;
        gameOver = false;
      } else if (ghostSettingsReleased) {
        ghostSettingsReleased = false;
        shouldDrawGhost = !shouldDrawGhost;
        shouldDrawPiece = !shouldDrawPiece;

        if (!shouldDrawGhost) {
          newDrawPixel(canvas, canvas->height()-1, canvas->width()-1, SETTINGS_VIOLET);
        } else {
          newDrawPixel(canvas, canvas->height()-1, canvas->width()-1, DIM_WHITE);
        }
      }
      if (millis() - gameOverAt > timeBeforeSleep) {
        // Go into a "sleep mode" after some time
        clearMatrix(canvas);
      }
    }

    if (!gameOver) {
       gameOverDrawn = false;
       if 
       (firstIteration) {
          tetrisEngine.prepareNewGame(shouldDrawPiece);

          // Draw border
          newDrawLine(canvas, 0, 4, 0, canvas->width()-1, DIM_WHITE);
          /* canvas->drawLine(3, 0, 3, canvas->height() - 1, DIM_WHITE); */
          newDrawLine(canvas, 0, 3, canvas->height() - 1, 3, DIM_WHITE);
          /* canvas->drawLine(3, canvas->height()-1, canvas->width()-1, canvas->height() - 1, DIM_WHITE); */
          newDrawLine(canvas, canvas->height()-1, 3, canvas->height()-1, canvas->width()-1, DIM_WHITE);

          clearBottom(canvas);

          // Draw line separating new pieces from help piece display
          newDrawLine(canvas, 5, 0, 5, 2, DIM_WHITE);
       }
      gameOver = tetrisEngine.loop(controls);
      if (gameOver) {
        gameOverAt = millis();
      }

      if (tetrisEngine.generationThisIteration || (tetrisEngine.pieceHeldThisIteration && !tetrisEngine.pieceHeldThisGame)) {
        printNextPieces(canvas);
      }

      if (tetrisEngine.pieceHeldThisIteration) {
        clearHeldPiece(canvas);
        Tetromino* heldPiece = tetrisEngine.heldPiece;
        /* int xOffset = heldPiece -> symbolNum == 5 ? 1 : 2; */
        for (int y = 0; y < heldPiece -> dimension; y++) {
          for (int x = 0; x < heldPiece -> dimension; x++) {
            if (heldPiece -> orientations[0][y][x] == 1) {
              newDrawPixel(canvas, x+1, 1-y, colorMap[heldPiece -> symbolNum]);
              /* drawSquareNew(canvas, x, y + tetrisEngine.fieldHeight - 1, colorMap[heldPiece -> symbolNum], 2, xOffset); */
            }
          }
        }

        /* if (!tetrisEngine.pieceHeldThisGame) { */
        /*   // We had to generate a new piece since this is the first hold of the game, */
        /*   // so we need to redraw held pieces. */
        /*   printNextPieces(canvas); */
        /* } */
      }

      // Print board
      if (tetrisEngine.drawAllThisIteration) {
        printWholeBoard(canvas);
        drawGhost(canvas);
      } else if (tetrisEngine.drawThisIteration) {

        // Draw the new piece area
        for (int i = 0; i < INDICES_TO_DRAW_LENGTH && tetrisEngine.indicesToDraw[i] != -1; i++) {
           int indexToDraw = tetrisEngine.indicesToDraw[i];
           int x = indexToDraw % tetrisEngine.fieldWidth;
           int y = (indexToDraw - x) / tetrisEngine.fieldWidth;
            
           int currentNum = tetrisEngine.matrixRepresentation[indexToDraw];
           int currentColorInd = currentNum;
           if (currentNum == CURRENT_PIECE_CHAR) {
             if (shouldDrawPiece || tetrisEngine.gameController.dropPressed || tetrisEngine.firstPiece) {
               currentColorInd = tetrisEngine.currentPiece -> symbolNum;
             } else {
               // Draw black if we don't want to draw the current piece
               currentColorInd = 0;
             }
           }

           /* int currentColorInd = currentNum == CURRENT_PIECE_CHAR ? tetrisEngine.currentPiece -> symbolNum : currentNum; */
           /* int currentColor = colorMap[currentColorInd]; */
           /* int currentColorInd = currentNum == CURRENT_PIECE_CHAR ? tetrisEngine.currentPiece -> symbolNum : currentNum; */
           MatrixRGBColor currentColor = colorMap[currentColorInd];
           drawSquareNew(canvas, x, y, currentColor, 3);
        }

        drawGhost(canvas);
      }

      firstIteration = false;
    }
  }

  return 0;
}
