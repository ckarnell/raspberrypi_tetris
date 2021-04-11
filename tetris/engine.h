/* NOTE TO SELF: Run with `g++ -std=c++11 -o engineprog engine.cpp && ./engineprog` */
/* Compile with debugger with `g++ -std=c++11 -g -o engineprog engine.cpp` */
/* Debug with `sudo gdb ./engineprog` */

#include <math.h>
#include "piece.h"
#include "controller.h"
/* using namespace std; */

#include <stdio.h>

const int LOCK_DOWN_TIMER = 1500;
const int INDICES_TO_DRAW_LENGTH = 10;

const int MAIN_MATRIX_HEIGHT = 20;
const int MAIN_MATRIX_WIDTH = 10;
const int BUFFER_ZONE_HEIGHT = 5;
const int BORDER_SIZE = 1;
const int CURRENT_PIECE_CHAR = 9;

float getSpeedInMillisecondsByLevel(int level)
{
  int lastLevel = level - 1;
  float timeInMilliseconds = pow((0.8 - (lastLevel*0.007)), lastLevel) * 1000;
  return timeInMilliseconds;
}

struct TSpinInfo {
  bool tSpin;
  bool miniTSpin;
};

class TetrisEngine {
  public:
    /* PieceBag bag; */
    GameController gameController = GameController(133);
    Tetromino* currentPiece;
    PieceBag bag;

    struct TSpinInfo tSpinInfo = {
      false,
      false,
    };

    long currentTime = millis();
    long lastDasAt = 0;
    int dasSpeed = 10;
    int softDropSpeed = 10;
    long lastSoftDropAt = 0;
    int rowsToRemove[4] = {-1, -1, -1, -1};
    int lastDraw = currentTime - 1001;
    bool shouldDrawPiece = true;
    bool gameOver = false;
    bool firstIteration = true;
    bool firstPiece = true;
    bool drawAllThisIteration = false;
    bool drawThisIteration = false;
    bool generationThisIteration;

    // Piece holding
    Tetromino* heldPiece;
    bool pieceHeldThisGame = false;
    bool pieceHeldThisIteration = false;
    bool pieceHeldThisPieceFall = false;

    // Score
    long long score = 0;
    int rowsCleared = 0;
    int currentLevel = 1;
    int rowsThisLevel = 0;
    bool backToBack = false;
    int rowsRemovedThisIteration = 0;
    int rowsSoftDroppedThisPiece = 0;
    int rowsHardDroppedThisPiece = 0;
    bool tspinThisPiece = false;

    // Drop
    int lastDrop = currentTime;
    float dropAfter = getSpeedInMillisecondsByLevel(currentLevel);

    // Lockdown
    int lockDownTimerMs = LOCK_DOWN_TIMER;
    long lockingDownAt = 0;
    int lockDownMaxY = -100;

    // Field
    int fieldWidth = MAIN_MATRIX_WIDTH + (BORDER_SIZE*2);
    int fieldHeight = MAIN_MATRIX_HEIGHT + BORDER_SIZE + BUFFER_ZONE_HEIGHT;

    // Falling piece state
    int currentX;
    int currentY;
    int orientation = 0;
    bool justLocked = false;

    // Board state
    unsigned char *matrixRepresentation = nullptr;
    int indicesToDraw[INDICES_TO_DRAW_LENGTH] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    int indForIndicesToDraw = 0;
    int pastCoordinates[4] = {-1, -1, -1, -1};
    // Use this to compute drop locations faster
    /* int lowestOccupiedYValues[12] = {-100, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, -100}; */

    /* void drawGhostOnBoard() { */
    /*   /1* return; *1/ */
    /*   int currentDimension = currentPiece -> dimension; */
    /*   int charToDraw = 10; */

    /*   // Undraw past ghost coordinates */
    /*   for (int i = 0; i < fieldWidth*fieldHeight; i++) { */
    /*     if (matrixRepresentation[i] == charToDraw) { */
    /*       matrixRepresentation[i] = 0; */
    /*       addIndexToDraw(i); */
    /*     } */
    /*   } */

    /*   int currentGhostY = getYModifierAfterHardDrop() + currentY; */

    /*   for (int y = 0; y < currentDimension; y++) { */
    /*     for (int x = 0; x < currentDimension; x++) { */
    /*       int minoRepresentation = currentPiece -> orientations[orientation][y][x]; */
    /*       if (minoRepresentation == 1) { */
    /*         matrixRepresentation[(y + currentGhostY)*fieldWidth + (x + currentX)] = charToDraw; */

    /*         // This is the first time this ghost is on the board, so draw it */
    /*         if (generationThisIteration) { */
    /*           addIndexToDraw((y + currentGhostY)*fieldWidth + (x + currentX)); */
    /*         } */
    /*       } */
    /*     } */
    /*   } */

    /*   // Mark the diff between what was on the board before and what is on the board now for printing */
    /*   /1* if (pastGhostCoordinates[0] != -1) { *1/ */
    /*   /1*   for (int i = 0; i < 4; i++) { *1/ */
    /*   /1*     bool newCoordNeedsDrawing = true; *1/ */
    /*   /1*     for (int j = 0; j < 4; j++) { *1/ */
    /*   /1*       if (newPastGhostCoordinates[i] == pastGhostCoordinates[j]) *1/ */
    /*   /1*         newCoordNeedsDrawing = false; *1/ */
    /*   /1*     } *1/ */
    /*   /1*     if (newCoordNeedsDrawing || pieceHeldThisIteration) *1/ */ 
    /*   /1*       addIndexToDraw(newPastGhostCoordinates[i]); *1/ */
    /*   /1*   } *1/ */

    /*   /1*   for (int i = 0; i < 4; i++) { *1/ */
    /*   /1*     bool pastCoordNeedsDrawing = true; *1/ */
    /*   /1*     for (int j = 0; j < 4; j++) { *1/ */
    /*   /1*       if (pastGhostCoordinates[i] == newPastGhostCoordinates[j]) *1/ */
    /*   /1*         pastCoordNeedsDrawing = false; *1/ */
    /*   /1*     } *1/ */
    /*   /1*     if (pastCoordNeedsDrawing || pieceHeldThisIteration) *1/ */ 
    /*   /1*       addIndexToDraw(pastGhostCoordinates[i]); *1/ */
    /*   /1*   } *1/ */
    /*   /1* } *1/ */

    /*   /1* // Finally, copy over *1/ */
    /*   /1* for (int i = 0; i < 4; i++) { *1/ */
    /*   /1*   pastGhostCoordinates[i] = newPastGhostCoordinates[i]; *1/ */
    /*   /1* } *1/ */
    /* } */

    void drawPieceOnBoard() {
      int currentDimension = currentPiece -> dimension;
      /* int pieceOrientation[4][4]; */
      /* currentPiece -> getOrientation(orientation, pieceOrientation); */

      // Undraw past coordinates for the current piece, if it has them.
      if (pastCoordinates[0] != -1) {
        for (int i = 0; i < 4; i++) {
          matrixRepresentation[pastCoordinates[i]] = 0;
        }
      }

      // Draw new coordinates and save them as past coordinates.
      // Also, if it's the first iteration of this piece being on the board,
      // add it's indices to the draw list
      int pastCoordInd = 0;
      int newPastCoordinates[4] = {-1, -1, -1, -1};
      for (int y = 0; y < currentDimension; y++) {
        for (int x = 0; x < currentDimension; x++) {
          int minoRepresentation = currentPiece -> orientations[orientation][y][x];
          if (minoRepresentation == 1) {
            int charToDraw = justLocked ? currentPiece -> symbolNum : CURRENT_PIECE_CHAR;

            if (justLocked && !shouldDrawPiece) {
              // Since we're not drawing the piece while falling, make sure to draw it when it's locked
              addIndexToDraw((y + currentY)*fieldWidth + (x + currentX));
            }

            matrixRepresentation[(y + currentY)*fieldWidth + (x + currentX)] = charToDraw;
            int pastCoord = justLocked ? -1 : (y + currentY)*fieldWidth + (x + currentX);
            newPastCoordinates[pastCoordInd] = pastCoord;

            // This is the first time this piece has been on the board, so draw it
            if (generationThisIteration) {
              addIndexToDraw((y + currentY)*fieldWidth + (x + currentX));
            }
            pastCoordInd++;
          }
        }
      }

      // Mark the diff between what was on the board before and what is on the board now for printing
      if (pastCoordinates[0] != -1) {
        for (int i = 0; i < 4; i++) {
          bool newCoordNeedsDrawing = true;
          for (int j = 0; j < 4; j++) {
            if (newPastCoordinates[i] == pastCoordinates[j])
              newCoordNeedsDrawing = false;
          }
          if (newCoordNeedsDrawing || pieceHeldThisIteration) 
            addIndexToDraw(newPastCoordinates[i]);
        }

        for (int i = 0; i < 4; i++) {
          bool pastCoordNeedsDrawing = true;
          for (int j = 0; j < 4; j++) {
            if (pastCoordinates[i] == newPastCoordinates[j])
              pastCoordNeedsDrawing = false;
          }
          if (pastCoordNeedsDrawing || pieceHeldThisIteration) 
            addIndexToDraw(pastCoordinates[i]);
        }
      }

      // Finally, copy over
      for (int i = 0; i < 4; i++) {
        pastCoordinates[i] = newPastCoordinates[i];
      }

    }

    /* void renderForCli() { */
    /*   for(int y = BUFFER_ZONE_HEIGHT; y < fieldHeight; y++) { */
    /*     /1* if (y % MAIN_MATRIX_HEIGHT == 0) { *1/ */
    /*     /1*   Serial.print("\n"); *1/ */
    /*     /1* } *1/ */

    /*     Serial.print(y); */
    /*     Serial.print(" "); */
    /*     for(int x = 0; x < fieldWidth; x++) { */
    /*       Serial.print(matrixRepresentation[y*fieldWidth + x]); */
    /*     } */
    /*     Serial.println(""); */
    /*   } */
    /*   Serial.println(""); */
    /* } */

    void addIndexToDraw(int indToDraw) {
      indicesToDraw[indForIndicesToDraw] = indToDraw;
      indForIndicesToDraw++;
      drawThisIteration = true;
    }

    bool pieceSetup() {
      orientation = 0;
      currentX = 4;

      // The I piece spawns one higher than the other pieces
      currentY = BUFFER_ZONE_HEIGHT - 2;

      if (isBlockedOut()) {
        return true;
      }

      // The piece is generated one space above the skyline. Check if it can move down immediately,
      // and if it can, move it down.
      if (!isCollisionDetected(currentX, currentY + 1, orientation)) {
        currentY++;
      }

      // Handle / reset lockdown timing
      if (isCollisionDetected(currentX, currentY + 1, orientation)) {
        lockDownMaxY = currentY;
        lockingDownAt = currentTime;
      } else {
        lockDownMaxY = -100;
        lockingDownAt = 0;
      }

      lockDownTimerMs = LOCK_DOWN_TIMER;
      lastDrop = currentTime;
      return false;
    }

    void generation() {
      currentPiece = bag.getNextPiece();
      gameOver = pieceSetup();

      // The game can end now if the newly generated piece overlaps with another one
      /* gameOver = isBlockedOut(); */
    }

    bool isCollisionDetected(int newX, int newY, int newOrientation) {
      bool collisionDetected = false;
      int currentDimension = currentPiece -> dimension;

      /* int newPieceOrientation[4][4] = currentPiece -> orientations[newOrientation]; */
      /* /1* currentPiece -> getOrientation(newOrientation, newPieceOrientation); *1/ */

      for (int y = 0; y < currentDimension; y++) {
        for (int x = 0; x < currentDimension; x++) {
          /* int oldMinoRepresentation = currentPieceOrientation[y][x]; */
          int newMinoRepresentation = currentPiece -> orientations[newOrientation][y][x];
          //  Check for 0 so we're not comparing squares that are occupied by the current piece
          int thisY = (y + newY)*fieldWidth;
          int thisX = x + newX;
          int matrixMino = matrixRepresentation[thisY + thisX];

          if (checkIfSquareIsOccupied(x + newX, y + newY) && matrixMino != CURRENT_PIECE_CHAR && newMinoRepresentation == 1) {
            // This is a part of the matrix that is actually occupied
            return true;
          }
        }
      }

      return collisionDetected;
    }

    void removeRows() {
      // Undraw rows queued for removal
      int numRowsToRemove = 0;
      for (int i = 0; i < 4; i++) {
        if (rowsToRemove[i] != -1) {
          numRowsToRemove++;
          for (int y = rowsToRemove[i]; y >= 0; y--) {
            for (int x = BORDER_SIZE; x < fieldWidth - BORDER_SIZE; x++) {
              int indToEdit = y*fieldWidth + x;
              if (y == 0) {
                matrixRepresentation[indToEdit] = 0;
              } else {
                matrixRepresentation[indToEdit] = matrixRepresentation[(y - 1)*fieldWidth + x];
              }
              /* } */
            }
          }
        }
        rowsToRemove[i] = -1;
      }

      if (numRowsToRemove != 0) {
        rowsCleared += numRowsToRemove;
        drawAllThisIteration = true;
      }
    }

    void queueRowsForRemoval() {
      int currentInd = 0;
      for (int y = currentY; y < currentY + currentPiece -> dimension; y++) {
        bool queueThisRow = true;
        for (int x = BORDER_SIZE; x < fieldWidth - BORDER_SIZE && queueThisRow; x++) {
          int minoRep = matrixRepresentation[y*fieldWidth + x];
          if (minoRep == 0 || minoRep == 1 || minoRep == 10) {
            // This slot is empty, so we don't queue this row
            queueThisRow = false;
          }
        }

        if (queueThisRow) {
          rowsRemovedThisIteration++;
          rowsToRemove[currentInd] = y;
          currentInd++;
        }
      }
    }

    void handleHold() {
      if (!gameController.holdPressed) {
        return;
      }

      if (pieceHeldThisPieceFall) {
        // You can only hold once per piece lock
        return;
      }

      pieceHeldThisIteration = true;
      pieceHeldThisPieceFall = true;
      drawAllThisIteration = firstIteration;

      if (pieceHeldThisGame) {
        // Swap the held piece with the current piece and reset state
        Tetromino* intermediatePiece = heldPiece;
        heldPiece = currentPiece;
        currentPiece = intermediatePiece;
        pieceSetup();
        return;
      }


      // Save the held piece, generate a new piece and reset state
      heldPiece = currentPiece;
      generation();
    }

    int getYModifierAfterHardDrop() {
        int yToDrop = 0;
        while (!isCollisionDetected(currentX, currentY + yToDrop + 1, orientation)) {
          yToDrop++;
        }
        return yToDrop;
    }

    void checkForPauseAndSelect() {
      if (gameController.selectPressed) {
        gameOver = true;
        return;
      }

      if (gameController.startPressed) {
        // TODO: Implement pausing
      }
    }
    void rotateAndMove() {
      if (gameController.dropPressed) {
        // Set currentY to the lowest Y below the current piece that is not occupied
        rowsHardDroppedThisPiece = getYModifierAfterHardDrop();
        currentY = currentY + rowsHardDroppedThisPiece;
        return;
      }

      if (gameController.leftDas) {
        if (currentTime - lastDasAt > dasSpeed) {
          lastDasAt = currentTime;

          if (!isCollisionDetected(currentX - 1, currentY, orientation)) {
            currentX--;
          }
        }
      } else if (gameController.leftPressed) {
        if (!isCollisionDetected(currentX - 1, currentY, orientation)) {
          currentX--;
        }
      }

      if (gameController.rightDas) {
        if (currentTime - lastDasAt > dasSpeed) {
          lastDasAt = currentTime;

          if (!isCollisionDetected(currentX + 1, currentY, orientation)) {
            currentX++;
          }
        }
      } else if (gameController.rightPressed) {
        if (!isCollisionDetected(currentX + 1, currentY, orientation)) {
          currentX++;
        }
      }

      if (gameController.clockwisePressed) {
        bool foundFittingPosition = false;
        int indexOfWorkingRotation = 0;
        int potentialNewOrientation = (orientation + 1) % 4;

        // Iterate through the possible ways we can rotate the piece until we find one or don't
        for (int i = 0; i < 5 && !foundFittingPosition; i++) {
          /* cout << possibleRotations[i][0] << possibleRotations[i][1] << endl; */
          foundFittingPosition = !isCollisionDetected(
              currentX + currentPiece -> offsets[orientation][1][i][0],
              currentY + currentPiece -> offsets[orientation][1][i][1],
              potentialNewOrientation
          );

          if (foundFittingPosition) {
            indexOfWorkingRotation = i;
          }
        }

        if (foundFittingPosition) {
          currentX += currentPiece -> offsets[orientation][1][indexOfWorkingRotation][0];
          currentY += currentPiece -> offsets[orientation][1][indexOfWorkingRotation][1];
          orientation = potentialNewOrientation;
        }
      }

      if (gameController.counterClockwisePressed) {
        bool foundFittingPosition = false;
        int indexOfWorkingRotation = 0;

        int potentialNewOrientation = orientation == 0 ? 3 : orientation - 1;
        if (potentialNewOrientation < 0) {
          potentialNewOrientation = 3;
        }

        // Iterate through the possible ways we can rotate the piece until we find one or don't
        for (int i = 0; i < 4 && !foundFittingPosition; i++) {
          /* cout << possibleRotations[i][0] << possibleRotations[i][1] << endl; */
          /* , currentX + currentPiece -> possibleRotations[i][0], currentY + currentPiece -> possibleRotations[i][1] */
          foundFittingPosition = !isCollisionDetected(
              currentX + currentPiece -> offsets[orientation][0][i][0],
              currentY + currentPiece -> offsets[orientation][0][i][1],
              potentialNewOrientation
          );

          if (foundFittingPosition) {
            indexOfWorkingRotation = i;
          }
        }

        if (foundFittingPosition) {
          currentX += currentPiece -> offsets[orientation][0][indexOfWorkingRotation][0];
          currentY += currentPiece -> offsets[orientation][0][indexOfWorkingRotation][1];
          orientation = potentialNewOrientation;
        }
      }

      if (gameController.flipPressed) {
        bool foundFittingPosition = false;
        int indexOfWorkingRotation = 0;

        int potentialNewOrientation = (orientation + 2) % 4;

        // Iterate through the possible ways we can rotate the piece until we find one or don't
        for (int i = 0; i < 4 && !foundFittingPosition; i++) {
          /* cout << possibleRotations[i][0] << possibleRotations[i][1] << endl; */
          /* , currentX + currentPiece -> possibleRotations[i][0], currentY + currentPiece -> possibleRotations[i][1] */
          foundFittingPosition = !isCollisionDetected(
              currentX + currentPiece -> offsets[orientation][0][i][0],
              currentY + currentPiece -> offsets[orientation][0][i][1],
              potentialNewOrientation
          );

          if (foundFittingPosition) {
            indexOfWorkingRotation = i;
          }
        }

        if (foundFittingPosition) {
          currentX += currentPiece -> offsets[orientation][0][indexOfWorkingRotation][0];
          currentY += currentPiece -> offsets[orientation][0][indexOfWorkingRotation][1];
          orientation = potentialNewOrientation;
        }
      }
    }

    bool isBlockedOut() {
      // This is a game over condition that occurs when a newly generated piece overlaps
      // with another piece.
      return isCollisionDetected(currentX, currentY, orientation);
    }

    bool isLockedOut() {
      // This is a game over condition that occurs when a piece locks without any of its
      // squares being in the game field, i.e. they're all in the buffer zone.
      if (!justLocked) {
        return false;
      }

      int currentDimension = currentPiece -> dimension;

      for (int y = 0; y < currentDimension; y++) {
        for (int x = 0; x < currentDimension; x++) {
          if (currentPiece -> orientations[orientation][y][x] == 1) {
            if (currentY + y >= BUFFER_ZONE_HEIGHT) {
              // This mino is in the game field, so there's no lockout
              return false;
            }
          }
        }
      }

      return true;
    }

    bool canPieceFall() {
      return !isCollisionDetected(currentX, currentY + 1, orientation);
    }

    bool shouldPieceLock() {
      if (gameController.dropPressed) {
        // Drop is pressed - we need to drop and lock on this iteration
        return true;
      } else if (canPieceFall()) {
        // Don't lock if there's nothing below the current piece - it has to fall
        return false;
      }
      return lockDownTimerMs <= 0;
    }

    bool shouldPieceTryToFall() {
      if (currentTime - lastDrop > dropAfter) {
        return true;
      }
      if (gameController.downHeld && (currentTime - lastSoftDropAt) > softDropSpeed) {
        lastSoftDropAt = currentTime;
        return true;
      }
      return false;
    }

    void handlePieceTryToFall() {
      // Modifies currentY, lockingDownAt, lastDrop, and rowsSoftDroppedThisPiece
      if (canPieceFall()) {
        // No collision, actually drop
        rowsSoftDroppedThisPiece += (gameController.downHeld && (currentTime - lastSoftDropAt) > softDropSpeed) ? 1 : 0;
        currentY++;
        lastDrop = currentTime;
        lockingDownAt = 0;
        return;
      }

      // We can't move down, so start the lockdown timer
      if (currentY > lockDownMaxY) {
        // Reset the clock if we fell since the last time the lockdown timer started
        lockDownTimerMs = LOCK_DOWN_TIMER;
      }
      lockingDownAt = currentTime;
      lockDownMaxY = currentY;
    }

    bool checkIfSquareIsOccupied(int x, int y) {
      int indToCheck = (y*fieldWidth) + x;
      /* return matrixRepresentation[indToCheck] > 0 && matrixRepresentation[indToCheck] < 10; */
      return matrixRepresentation[indToCheck] != 0 && matrixRepresentation[indToCheck] != 10;
    }

    void updateTspinInfo() {
      // Updates tSpinInfo
      if (!(currentPiece -> symbolNum == 2)) {
        return;
      }

      if (tSpinInfo.tSpin) {
        // We already know this is a t-spin because rotation point 5 was used to rotate
        // the piece
        return;
      }

      int A[2] = {
        currentX + (orientation == 0 || orientation == 3 ? 0 : 2),
        currentY + (orientation < 2 ? 0 : 2),
      };
      int B[2] = {
        currentX + (orientation < 2 ? 2 : 0),
        currentY + (orientation == 0 || orientation == 3 ? 0 : 2),
      };
      int C[2] = {
        currentX + (orientation < 2 ? 0 : 2), 
        currentY + (orientation == 0 || orientation == 3 ? 2 : 0),
      };
      int D[2] = {
        currentX + (orientation == 0 || orientation == 3 ? 2 : 0),
        currentY + (orientation < 2 ? 2 : 0), 
      };

      bool AContact = checkIfSquareIsOccupied(A[0], A[1]);
      bool BContact = checkIfSquareIsOccupied(B[0], B[1]);
      bool CContact = checkIfSquareIsOccupied(C[0], C[1]);
      bool DContact = checkIfSquareIsOccupied(D[0], D[1]);

      if ((AContact && BContact) && (CContact || DContact)) {
        tSpinInfo.tSpin = true;
        return;
      }

      if ((CContact && DContact) && (AContact || BContact)) {
        tSpinInfo.miniTSpin = true;
        return;
      }
    }

    void updateScoreAndBackToBack() {
      int numRowsToAdd = 0;
      // Updates backToBack, score, and tSpinInfo
      updateTspinInfo();
      bool isTspin = tSpinInfo.tSpin;
      bool isMiniTspin = tSpinInfo.miniTSpin;
      int scoreToAdd = 0;

      float backToBackMultiplier = backToBack ? 0.5 : 0;

      if (rowsRemovedThisIteration == 4 || (isTspin && rowsRemovedThisIteration > 0)) {
        backToBack = true;
      } else {
        backToBack = false;
      }

      switch (rowsRemovedThisIteration) {
        case 0:
          if (isTspin) {
            scoreToAdd += 400*currentLevel;
            numRowsToAdd += 4;
          } else if (isMiniTspin) {
            scoreToAdd += 100*currentLevel;
            numRowsToAdd += 1;
          } else {
            scoreToAdd = (rowsSoftDroppedThisPiece * 1) + (rowsHardDroppedThisPiece * 2);
          }
          break;
        case 1: // Single
          if (isTspin) {
            numRowsToAdd += 8;
            scoreToAdd += 800*currentLevel;
          } else if (isMiniTspin) {
            numRowsToAdd += 2;
            scoreToAdd += 200*currentLevel;
          } else {
            numRowsToAdd += 1;
            scoreToAdd += 100*currentLevel;
          }
          break;
        case 2: // Double
          if (isTspin) {
            scoreToAdd += 1200*currentLevel;
            numRowsToAdd += 12;
          } else  {
            scoreToAdd += 300*currentLevel;
            numRowsToAdd += 3;
          }
          break;
        case 3: // Triple
          if (isTspin) {
            scoreToAdd += 1200*currentLevel;
            numRowsToAdd += 16;
          } else  {
            scoreToAdd += 300*currentLevel;
            numRowsToAdd += 5;
          }
          break;
        case 4: // Tetris
          scoreToAdd += 800*currentLevel;
            numRowsToAdd += 8;
          break;
      }

      rowsThisLevel += numRowsToAdd + numRowsToAdd*backToBackMultiplier;
      score += scoreToAdd + scoreToAdd*backToBackMultiplier;
    }

    void updateLockDownTimer() {
      if (lockingDownAt != 0) {
        lockDownTimerMs -=  currentTime - lockingDownAt;
        lockingDownAt = currentTime;
      }
    }

    bool loop(Controls controls) {
      // Returns true if the game continues after the loop, false if there's a game over
      drawAllThisIteration = firstIteration;
      drawThisIteration = false;
      indForIndicesToDraw = 0;
      generationThisIteration = false;
      if (pieceHeldThisIteration) {
        pieceHeldThisIteration = false;
        pieceHeldThisGame = true;
      }

      rowsRemovedThisIteration = 0;

      for (int x = 0; x < 10; x++)
        indicesToDraw[x] = -1;

      currentTime = millis();

      if (justLocked) {
        firstPiece = false;
      }

      if (justLocked || firstIteration) {
        generationThisIteration = true;
        generation();
        if (gameOver) {
          return gameOver;
        }
        justLocked = false;
        firstIteration = false;
      }

      updateLockDownTimer();

      if (shouldPieceLock()) {
        justLocked = true;
        pieceHeldThisPieceFall = false;

        // The game could be over if we just locked a piece
      } else if (shouldPieceTryToFall()) {
        handlePieceTryToFall();
      }

      // Update and respond to input
      gameController.updateControls(controls, currentTime);
      checkForPauseAndSelect();

      if (!justLocked) {
        handleHold();
        if (gameOver) {
          return gameOver;
        }
        rotateAndMove();
      }

      drawPieceOnBoard();

      if (justLocked) {
        queueRowsForRemoval();

        updateScoreAndBackToBack();
        rowsSoftDroppedThisPiece = 0;
        rowsHardDroppedThisPiece = 0;
        tSpinInfo.tSpin = false;
        tSpinInfo.miniTSpin = false;

        removeRows();

        // There may be a game over at this point
        gameOver = isLockedOut();
        if (gameOver) {
          return gameOver;
        }

        // Decide whether to increase the level and difficulty
        if (currentLevel < 15 && rowsThisLevel >= currentLevel*5) {
        /* if (currentLevel < 15 && rowsCleared >= currentLevel*10) { */
          currentLevel++;
          rowsThisLevel = 0;
          dropAfter = getSpeedInMillisecondsByLevel(currentLevel);
        }
      }

      return gameOver;
    }

    void createNewPlayField() {
      for (int y = 0; y < fieldHeight; y++) {
        for (int x = 0; x < fieldWidth; x++) { // Board Boundary
          // "1" represents a border, "0" is anything else.
          matrixRepresentation[y*fieldWidth + x] = 0;
          if (x == 0) {
            // This is the first element of the line, it's a border character.
            matrixRepresentation[y*fieldWidth + x] = 1;
          } else if (x == fieldWidth - 1) {
            // This is the last element of the line, it's a border character.
            matrixRepresentation[y*fieldWidth + x] = 1;
          } else if (y == fieldHeight - 1) {
            // This is the last row of the board, every character on this row is a border character.
            matrixRepresentation[y*fieldWidth + x] = 1;
          }
        }
      }
    }

    void prepareNewGame(bool shouldDrawPieceThisGame) {
      // Reset state
      firstPiece = true;
      shouldDrawPiece = shouldDrawPieceThisGame;
      gameOver = false;
      rowsThisLevel = 0;
      createNewPlayField();
      firstIteration = true;
      justLocked = false;
      bag.createNewBagOrder(true);
      bag.currentIndex = 0;

      pieceHeldThisIteration = false;
      pieceHeldThisPieceFall = false;
      pieceHeldThisGame = false;

      score = 0;
      currentLevel = 1;
      rowsCleared = 0;
      dropAfter = getSpeedInMillisecondsByLevel(currentLevel);
    }

    TetrisEngine() {
      random();
      bag = PieceBag();

      // Create initial board representation, which is empty except for boarders.
      // Note that this includes a "buffer zone" above the main viewable board,
      // which won't be rendered, but is important for game state management.
      matrixRepresentation = new unsigned char[fieldWidth*fieldHeight]; // Create play field buffer
    }
};
