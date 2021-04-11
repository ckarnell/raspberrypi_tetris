#ifndef MYFILE_H_
#define MYFILE_H_

/* #include <Vector.h> */
/* #include <string> */
/* #include <random> */
/* using namespace std; // TODO: Neeeded? */


struct Tetromino {
  char symbol;
  int symbolNum;
  int dimension;
  int orientations[4][4][4];
  int offsets[4][2][5][2];
};

struct Tetromino TPiece = {
  'T',
  2,
  3,
  {
    {
        { 0, 1, 0},
        { 1, 1, 1},
        { 0, 0, 0},
    },
    {
        { 0, 1, 0},
        { 0, 1, 1},
        { 0, 1, 0},
    },
    {
        { 0, 0, 0},
        { 1, 1, 1},
        { 0, 1, 0},
    },
    {
        { 0, 1, 0},
        { 1, 1, 0},
        { 0, 1, 0},
    }
  },
  {
    // North
    {
      // Counter clockwise
      {
        {0, 0}, {1, 0}, {1, -1}, {1, 2}, {0, 0},
      },
      // Clockwise
      {
        {0, 0}, {-1, 0}, {-1, -1}, {-1, 2}, {0, 0},
      },
    },
    // East
    {
      // Counter clockwise
      {
        {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
      },
      // Clockwise
      {
        {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
      },
    },
    // South
    {
      // Counter clockwise
      {
        {0, 0}, {-1, 0}, {0, 2}, {-1, 2}, {0, 0},
      },
      // Clockwise
      {
        {0, 0}, {1, 0}, {0, 2}, {1, 2}, {0, 0},
      },
    },
    // West
    {
      // Counter clockwise
      {
        {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
      },
      // Clockwise
      {
        {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
      },
    },
  },
};

struct Tetromino SPiece = {
  'S',
  3,
  3,
  {
    {
        { 0, 1, 1},
        { 1, 1, 0},
        { 0, 0, 0},
    },
    {
        { 0, 1, 0},
        { 0, 1, 1},
        { 0, 0, 1},
    },
    {
        { 0, 0, 0},
        { 0, 1, 1},
        { 1, 1, 0},
    },
    {
        { 1, 0, 0},
        { 1, 1, 0},
        { 0, 1, 0},
    }
  },
  {
    // North
    {
      // Counter clockwise
      {
        {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2},
      },
      // Clockwise
      {
        {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2},
      },
    },
    // East
    {
      // Counter clockwise
      {
        {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
      },
      // Clockwise
      {
        {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
      },
    },
    // South
    {
      // Counter clockwise
      {
        {0, 0}, {-1, 0}, {-1, 1}, {0, 2}, {-1, 2},
      },
      // Clockwise
      {
        {0, 0}, {1, 0}, {1, 1}, {0, 2}, {1, 2},
      },
    },
    // West
    {
      // Counter clockwise
      {
        {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
      },
      // Clockwise
      {
        {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
      },
    },
  },
};

struct Tetromino ZPiece = {
  'Z',
  4,
  3,
  {
    {
        { 1, 1, 0},
        { 0, 1, 1},
        { 0, 0, 0},
    },
    {
        { 0, 0, 1},
        { 0, 1, 1},
        { 0, 1, 0},
    },
    {
        { 0, 0, 0},
        { 1, 1, 0},
        { 0, 1, 1},
    },
    {
        { 0, 1, 0},
        { 1, 1, 0},
        { 1, 0, 0},
    }
  },
  {
    // North
    {
      // Counter clockwise
      {
        {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2},
      },
      // Clockwise
      {
        {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 1},
      },
    },
    // East
    {
      // Counter clockwise
      {
        {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
      },
      // Clockwise
      {
        {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
      },
    },
    // South
    {
      // Counter clockwise
      {
        {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2},
      },
      // Clockwise
      {
        {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2},
      },
    },
    // West
    {
      // Counter clockwise
      {
        {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
      },
      // Clockwise
      {
        {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
      },
    }
  },
};

struct Tetromino IPiece = {
  'I',
  5,
  4,
  {
    {
        { 0, 0, 0, 0 },
        { 1, 1, 1, 1 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 }
    },
    {
        { 0, 0, 1, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 1, 0 }
    },
    {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 1, 1, 1, 1 },
        { 0, 0, 0, 0 }
    },
    {
        { 0, 1, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 1, 0, 0 }
    }
  },
  {
    // North
    {
      // Counter clockwise
      {
        {0, 0}, {-1, 0}, {2, 0}, {-1, -2}, {2, 1},
      },
      // Clockwise
      {
        {0, 0}, {-2, 0}, {1, 0}, {-2, 1}, {1, -2},
      },
    },
    // East
    {
      // Counter clockwise
      {
        {0, 0}, {2, 0}, {-1, 0}, {2, -1}, {-1, 2},
      },
      // Clockwise
      {
        {0, 0}, {-1, 0}, {2, 0}, {-1, -2}, {2, 1},
      },
    },
    // South
    {
      // Counter clockwise
      {
        {0, 0}, {1, 0}, {-2, 0}, {1, 2}, {-2, -1},
      },
      // Clockwise
      {
        {0, 0}, {2, 0}, {-1, 0}, {2, -1}, {-1, 2},
      },
    },
    // West
    {
      // Counter clockwise
      {
        {0, 0}, {-2, 0}, {1, 0}, {-2, 1}, {1, -2},
      },
      // Clockwise
      {
        {0, 0}, {1, 0}, {-2, 0}, {1, 2}, {-2, -1},
      },
    },
  },
};

struct Tetromino LPiece = {
  'L',
  6,
  3,
  {
    {
        { 0, 0, 1},
        { 1, 1, 1},
        { 0, 0, 0},
    },
    {
        { 0, 1, 0},
        { 0, 1, 0},
        { 0, 1, 1},
    },
    {
        { 0, 0, 0},
        { 1, 1, 1},
        { 1, 0, 0},
    },
    {
        { 1, 1, 0},
        { 0, 1, 0},
        { 0, 1, 0},
    },
  },
  {
    // North
    {
      // Counter clockwise
      {
        {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2},
      },
      // Clockwise
      {
        {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2},
      },
    },
    // East
    {
      // Counter clockwise
      {
        {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
      },
      // Clockwise
      {
        {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
      },
    },
    // South
    {
      // Counter clockwise
      {
        {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2},
      },
      // Clockwise
      {
        {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2},
      },
    },
    // West
    {
      // Counter clockwise
      {
        {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
      },
      // Clockwise
      {
        {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
      },
    },
  },
};

struct Tetromino JPiece = {
  'J',
  7,
  3,
  {
    {
        { 1, 0, 0 },
        { 1, 1, 1 },
        { 0, 0, 0 },
    },
    {
        { 0, 1, 1 },
        { 0, 1, 0 },
        { 0, 1, 0 },
    },
    {
        { 0, 0, 0 },
        { 1, 1, 1 },
        { 0, 0, 1 },
    },
    {
        { 0, 1, 0 },
        { 0, 1, 0 },
        { 1, 1, 0 },
    },
  },
  {
    // North
    {
      // Counter clockwise
      {
        {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2},
      },
      // Clockwise
      {
        {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2},
      },
    },
    // East
    {
      // Counter clockwise
      {
        {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
      },
      // Clockwise
      {
        {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2},
      },
    },
    // South
    {
      // Counter clockwise
      {
        {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2},
      },
      // Clockwise
      {
        {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2},
      },
    },
    // West
    {
      // Counter clockwise
      {
        {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
      },
      // Clockwise
      {
        {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2},
      },
    },
  },
};

struct Tetromino OPiece = {
  'O',
  8,
  3,
  {
    {
        { 0, 1, 1 },
        { 0, 1, 1 },
        { 0, 0, 0 },
    },
    {
        { 0, 1, 1 },
        { 0, 1, 1 },
        { 0, 0, 0 },
    },
    {
        { 0, 1, 1 },
        { 0, 1, 1 },
        { 0, 0, 0 },
    },
    {
        { 0, 1, 1 },
        { 0, 1, 1 },
        { 0, 0, 0 },
    }
    /* { */
    /*     { 1, 1 }, */
    /*     { 1, 1 }, */
    /* }, */
    /* { */
    /*     { 1, 1 }, */
    /*     { 1, 1 }, */
    /* }, */
    /* { */
    /*     { 1, 1 }, */
    /*     { 1, 1 }, */
    /* }, */
    /* { */
    /*     { 1, 1 }, */
    /*     { 1, 1 }, */
    /* } */
  },
  {
    // North
    {
      // Clockwise
      {
        {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
      },
      // Counter clockwise
      {
        {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
      },
    },
    // East
    {
      // Clockwise
      {
        {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
      },
      // Counter clockwise
      {
        {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
      },
    },
    // South
    {
      // Clockwise
      {
        {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
      },
      // Counter clockwise
      {
        {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
      },
    },
    // West
    {
      // Clockwise
      {
        {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
      },
      // Counter clockwise
      {
        {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
      },
    },
  },
};

void shuffleArray(int * array, int size)
{
  int last = 0;
  int temp = array[last];
  for (int i=0; i<size; i++)
  {
    int index = rand() % size;
    array[last] = array[index];
    last = index;
  }
  array[last] = temp;
}

const int NUMBER_OF_TETROMINOS = 7;

class PieceBag {
  public:
    int currentIndex = 0;
    int bagOrder[14];
    Tetromino* tetrominos[NUMBER_OF_TETROMINOS];

    void createNewBagOrder(bool firstIteration) 
    {
      // Shuffle two bags separately, then append them to each other.
      // We create two at once so we can always see what pieces are coming up.

      int orderedBag1[NUMBER_OF_TETROMINOS] = {0, 1, 2, 3, 4, 5, 6};
      shuffleArray(orderedBag1, NUMBER_OF_TETROMINOS);

      if (firstIteration) {
        /* std::random_shuffle (orderedBag1.begin(), orderedBag1.end(), random); */

        for (int i = 0; i < NUMBER_OF_TETROMINOS; i++) {
          bagOrder[i] = orderedBag1[i];
        }

        for (int i = 0; i < NUMBER_OF_TETROMINOS; i++) {
          orderedBag1[i] = i;
        }
        shuffleArray(orderedBag1, NUMBER_OF_TETROMINOS);
      } else {
        for (int i = 0; i < NUMBER_OF_TETROMINOS; i++) {
          bagOrder[i] = bagOrder[NUMBER_OF_TETROMINOS + i];
        }
      }

      /* std::random_shuffle (orderedBag2.begin(), orderedBag2.end(), random); */


      for (int i = 0; i < NUMBER_OF_TETROMINOS; i++) {
        bagOrder[i + NUMBER_OF_TETROMINOS] = orderedBag1[i];
      }
    }

    Tetromino* getNextPiece() {
      /* cout << currentIndex << endl; */
      if (currentIndex == NUMBER_OF_TETROMINOS)
      {
        currentIndex = 0;
        createNewBagOrder(false); // Mutates bagOrder
      }

      Tetromino* tetrominoToReturn = tetrominos[bagOrder[currentIndex]];
      currentIndex++;
      return tetrominoToReturn;
    }

    Tetromino* getFuturePiece(int ind) {
      Tetromino* tetrominoToReturn = tetrominos[bagOrder[currentIndex + ind - 1]];
      return tetrominoToReturn;
    }

    PieceBag() {
      // This next line seems necessary to "kick off" actual randomness,
      // otherwise the first piece is always the same. Very weird.
      /* srand (time(NULL)); */
      /* randomNumber(7); */

      /* random(10); */
      /* random(10); */
      tetrominos[0] = &TPiece;
      tetrominos[1] = &SPiece;
      tetrominos[2] = &ZPiece;
      tetrominos[3] = &JPiece;
      tetrominos[4] = &LPiece;
      tetrominos[5] = &IPiece;
      tetrominos[6] = &OPiece;

      /* createNewBagOrder(true); */
    }
};

#endif
