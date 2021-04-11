#include <chrono>
using namespace std::chrono;

long millis() {
  return duration_cast< milliseconds >(
    system_clock::now().time_since_epoch()
  ).count();
}

// Struct of all the actual inputs to the game
struct Controls {
  bool right;
  bool left;
  bool up;
  bool down;
  bool rotateClockwise;
  bool rotateCounterClockwise;
  bool flip;
  bool hold;
  bool select;
  bool start;
};

class GameController {
  public:
    bool leftPressed;
    bool selectPressed = false;
    bool startPressed = false;
    bool rightPressed;
    bool leftDas;
    bool rightDas;
    int dropPressed;
    int downHeld;
    bool clockwisePressed = false;
    bool holdPressed = false;
    bool counterClockwisePressed = false;
    bool flipPressed = false;

    GameController(int dasDelay) {
      _dasDelay = dasDelay;
      _currentTime = millis();

      // "Pressed" means pressed THIS ITERATION - i.e. newly pressed
      leftPressed = false;
      rightPressed = false;
      leftDas = false;
      rightDas = false;
      dropPressed = false;
      holdPressed = false;
      // 0 is a sentinal value here, implying the button isn't pushed at all
      leftPressedAt = 0;
      rightPressedAt = 0;
    }

    void updateControls(Controls controls, int currentTime) {
      selectPressed = controls.select;
      startPressed = controls.start;
      downHeld = controls.down;

      if (controls.left) {
        if (leftPressedAt == 0) {
          // It was pressed this iteration
          leftPressed = true;
          leftPressedAt = currentTime;
        } else {
          // Left was not pressed this iteration, set value to false
          leftPressed = false;

          // That means this was held - check for das
          if (leftPressedAt != 0 && (currentTime - leftPressedAt > _dasDelay)) {
            // If both right and left are held, but right was pushed later, don't turn
            // on left das
            leftDas = !(rightPressedAt != 0 && rightPressedAt > leftPressedAt);
          }
        }
      } else {
        leftDas = false;
        leftPressed = false;
        leftPressedAt = 0;
      }

      if (controls.right) {
        if (rightPressedAt == 0) {
          // It was pressed this iteration
          rightPressed = true;
          rightPressedAt = currentTime;
        } else {
          // Right was not pressed this iteration, set value to false
          rightPressed = false;

          // That means this was held - check for das
          if (rightPressedAt != 0 && (currentTime - rightPressedAt > _dasDelay)) {
            // If both right and left are held, but left was pushed later, don't turn
            // on right das
            rightDas = !(leftPressedAt != 0 && leftPressedAt > rightPressedAt);
          }
        }
      } else {
        rightDas = false;
        rightPressed = false;
        rightPressedAt = 0;
      }

      if (controls.rotateClockwise) {
        if (!clockwiseHeld) {
          clockwisePressed = true;
          clockwiseHeld = true;
        } else {
          clockwisePressed = false;
        }
      } else {
        clockwisePressed = false;
        clockwiseHeld = false;
      }


      if (controls.rotateCounterClockwise) {
        if (!counterClockwiseHeld) {
          counterClockwisePressed = true;
          counterClockwiseHeld = true;
        } else {
          counterClockwisePressed = false;
        }
      } else {
        counterClockwisePressed = false;
        counterClockwiseHeld = false;
      }


      if (controls.up) {
        if (!dropHeld) {
          dropHeld = true;
          dropPressed = true;
        } else {
          dropPressed = false;
        }
      } else {
        dropHeld = false;
        dropPressed = false;
      }

      if (controls.flip) {
        if (!flipHeld) {
          flipHeld = true;
          flipPressed = true;
        } else {
          flipPressed = false;
        }
      } else {
        flipHeld = false;
        flipPressed = false;
      }

      if (controls.hold) {
        if (!holdHeld) {
          holdPressed = true;
          holdHeld = true;
        } else {
          holdPressed = false;
        }
      } else {
        holdPressed = false;
        holdHeld = false;
      }
    }

  private:
    long _currentTime;
    long leftPressedAt;
    long rightPressedAt;
    int clockwiseHeld;
    int holdHeld = false;
    int counterClockwiseHeld;
    bool dropHeld = false;
    int flipHeld = false;
    int _dasDelay;
};
