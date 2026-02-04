#include <climits>
#include <iostream>
#include <vector>

uint64_t whitePawns = 0xff00;
uint64_t whiteKnights = 0x42;
uint64_t whiteBishops = 0x24;
uint64_t whiteRooks = 0x81;
uint64_t whiteQueens = 0x10;
uint64_t whiteKings = 0x8;

uint64_t blackPawns = 0xff000000000000;
uint64_t blackKnights = 0x4200000000000000;
uint64_t blackBishops = 0x2400000000000000;
uint64_t blackRooks = 0x8100000000000000;
uint64_t blackQueens = 0x1000000000000000;
uint64_t blackKings = 0x800000000000000;

uint64_t whitePieces = (whitePawns | whiteKnights | whiteBishops | whiteRooks |
                        whiteQueens | whiteKings);
uint64_t blackPieces = (blackPawns | blackKnights | blackBishops | blackRooks |
                        blackQueens | blackKings);

const uint64_t BOTTOM_RANK = 0xff;
const uint64_t SECOND_RANK = 0xff00;
const uint64_t SEVENTH_RANK = 0xff000000000000;
const uint64_t TOP_RANK = 0xff00000000000000;

const uint64_t LEFT_FILE = 0x8080808080808080;
const uint64_t SECOND_FILE = 0x4040404040404040;
const uint64_t SEVENTH_FILE = 0x0202020202020202;
const uint64_t RIGHT_FILE = 0x0101010101010101;

std::vector<int> directions = {-8, 8, -1, 1, -9, 9, -7, 7};

std::vector<uint64_t> endTiles = {BOTTOM_RANK,
                                  TOP_RANK,
                                  RIGHT_FILE,
                                  LEFT_FILE,
                                  (BOTTOM_RANK | RIGHT_FILE),
                                  (TOP_RANK | LEFT_FILE),
                                  (BOTTOM_RANK | LEFT_FILE),
                                  (TOP_RANK | RIGHT_FILE)};

std::vector<uint64_t *> pieces = {&whitePawns, &whiteKnights, &whiteBishops,
                                  &whiteRooks, &whiteQueens,  &whiteKings,
                                  &blackPawns, &blackKnights, &blackBishops,
                                  &blackRooks, &blackQueens,  &blackKings};

class Move {
public:
  uint64_t from;
  uint64_t to;
  uint64_t *movedPiece;
  uint64_t *capturedPiece;

  Move(uint64_t f, uint64_t t, uint64_t *mp, uint64_t *cp)
      : from(f), to(t), movedPiece(mp), capturedPiece(cp) {}
};

uint64_t *findPiece(uint64_t tile) {
  for (int i = 0; i < 12; i++) {
    if (*pieces[i] & tile) {
      return pieces[i];
    }
  }
  return 0;
}

int getScore() {

  int score = 0;
  for (int r = 7; r >= 0; r--) {
    for (int c = 7; c >= 0; c--) {

      uint64_t tile = (1ULL << c) << (r * 8);

      if (whitePawns & tile) {
        score += 100;
      } else if (blackPawns & tile) {
        score -= 100;
      } else if (whiteKnights & tile) {
        score += 300;
      } else if (blackKnights & tile) {
        score -= 300;
      } else if (whiteBishops & tile) {
        score += 300;
      } else if (blackBishops & tile) {
        score -= 300;
      } else if (whiteRooks & tile) {
        score += 500;
      } else if (blackRooks & tile) {
        score -= 500;
      } else if (whiteQueens & tile) {
        score += 900;
      } else if (blackQueens & tile) {
        score -= 900;
      }
    }
  }
  return score;
}

void printBoard() {
  for (int r = 7; r >= 0; r--) {
    for (int c = 7; c >= 0; c--) {

      uint64_t tile = (1ULL << c) << (r * 8);

      if (whitePawns & tile) {
        std::cout << 'P' << ' ';
      } else if (blackPawns & tile) {
        std::cout << 'p' << ' ';
      } else if (whiteKnights & tile) {
        std::cout << 'N' << ' ';
      } else if (blackKnights & tile) {
        std::cout << 'n' << ' ';
      } else if (whiteBishops & tile) {
        std::cout << 'B' << ' ';
      } else if (blackBishops & tile) {
        std::cout << 'b' << ' ';
      } else if (whiteRooks & tile) {
        std::cout << 'R' << ' ';
      } else if (blackRooks & tile) {
        std::cout << 'r' << ' ';
      } else if (whiteQueens & tile) {
        std::cout << 'Q' << ' ';
      } else if (blackQueens & tile) {
        std::cout << 'q' << ' ';
      } else if (whiteKings & tile) {
        std::cout << 'K' << ' ';
      } else if (blackKings & tile) {
        std::cout << 'k' << ' ';
      } else {
        std::cout << '.' << ' ';
      }
    }
    std::cout << "\n";
  }
}

void generatePawnMoves(std::vector<Move> *moves, bool color,
                       uint64_t startTile) {
  if (color) {
    if (!(startTile & TOP_RANK) &&
        !((startTile << 8) & (whitePieces | blackPieces))) {
      moves->push_back(Move(startTile, startTile << 8, &whitePawns, 0));
      if ((startTile & SECOND_RANK) &&
          !((startTile << 16) & (whitePieces | blackPieces))) {
        moves->push_back(Move(startTile, startTile << 16, &whitePawns, 0));
      }
    }
    if (!(startTile & TOP_RANK) && !(startTile & LEFT_FILE) &&
        ((startTile << 9) & blackPieces)) {
      moves->push_back(Move(startTile, startTile << 9, &whitePawns,
                            findPiece(startTile << 9)));
    }
    if (!(startTile & TOP_RANK) && !(startTile & RIGHT_FILE) &&
        ((startTile << 7) & blackPieces)) {
      moves->push_back(Move(startTile, startTile << 7, &whitePawns,
                            findPiece(startTile << 7)));
    }
  } else {
    if (!(startTile & BOTTOM_RANK) &&
        !((startTile >> 8) & (whitePieces | blackPieces))) {
      moves->push_back(Move(startTile, startTile >> 8, &blackPawns, 0));
      if ((startTile & SEVENTH_RANK) &&
          !((startTile >> 16) & (whitePieces | blackPieces))) {
        moves->push_back(Move(startTile, startTile >> 16, &blackPawns, 0));
      }
    }
    if (!(startTile & BOTTOM_RANK) && !(startTile & LEFT_FILE) &&
        ((startTile >> 7) & whitePieces)) {
      moves->push_back(Move(startTile, startTile >> 7, &blackPawns,
                            findPiece(startTile >> 7)));
    }
    if (!(startTile & BOTTOM_RANK) && !(startTile & RIGHT_FILE) &&
        ((startTile >> 9) & whitePieces)) {
      moves->push_back(Move(startTile, startTile >> 9, &blackPawns,
                            findPiece(startTile >> 9)));
    }
  }
}

void generateKnightMoves(std::vector<Move> *moves, bool color,
                         uint64_t startTile) {
  if (!((SEVENTH_RANK | TOP_RANK) & startTile) && !(LEFT_FILE & startTile) &&
      !((startTile << 17) & (color ? whitePieces : blackPieces))) {
    moves->push_back(Move(startTile, startTile << 17,
                          (color ? &whiteKnights : &blackKnights),
                          findPiece(startTile << 17)));
  }
  if (!((SEVENTH_RANK | TOP_RANK) & startTile) && !(RIGHT_FILE & startTile) &&
      !((startTile << 15) & (color ? whitePieces : blackPieces))) {
    moves->push_back(Move(startTile, startTile << 15,
                          (color ? &whiteKnights : &blackKnights),
                          findPiece(startTile << 15)));
  }
  if (!((SEVENTH_FILE | RIGHT_FILE) & startTile) && !(TOP_RANK & startTile) &&
      !((startTile << 6) & (color ? whitePieces : blackPieces))) {
    moves->push_back(Move(startTile, startTile << 6,
                          (color ? &whiteKnights : &blackKnights),
                          findPiece(startTile << 6)));
  }
  if (!((SEVENTH_FILE | RIGHT_FILE) & startTile) &&
      !(BOTTOM_RANK & startTile) &&
      !((startTile >> 10) & (color ? whitePieces : blackPieces))) {
    moves->push_back(Move(startTile, startTile >> 10,
                          (color ? &whiteKnights : &blackKnights),
                          findPiece(startTile >> 10)));
  }
  if (!((SECOND_RANK | BOTTOM_RANK) & startTile) && !(RIGHT_FILE & startTile) &&
      !((startTile >> 17) & (color ? whitePieces : blackPieces))) {
    moves->push_back(Move(startTile, startTile >> 17,
                          (color ? &whiteKnights : &blackKnights),
                          findPiece(startTile >> 17)));
  }
  if (!((SECOND_RANK | BOTTOM_RANK) & startTile) && !(LEFT_FILE & startTile) &&
      !((startTile >> 15) & (color ? whitePieces : blackPieces))) {
    moves->push_back(Move(startTile, startTile >> 15,
                          (color ? &whiteKnights : &blackKnights),
                          findPiece(startTile >> 15)));
  }
  if (!((SECOND_FILE | LEFT_FILE) & startTile) && !(BOTTOM_RANK & startTile) &&
      !((startTile >> 6) & (color ? whitePieces : blackPieces))) {
    moves->push_back(Move(startTile, startTile >> 6,
                          (color ? &whiteKnights : &blackKnights),
                          findPiece(startTile >> 6)));
  }
  if (!((SECOND_FILE | LEFT_FILE) & startTile) && !(TOP_RANK & startTile) &&
      !((startTile << 10) & (color ? whitePieces : blackPieces))) {
    moves->push_back(Move(startTile, startTile << 10,
                          (color ? &whiteKnights : &blackKnights),
                          findPiece(startTile << 10)));
  }
}

void generateSlidingMoves(std::vector<Move> *moves, bool color,
                          uint64_t startTile) {
  int startDir = 0;
  int endDir = 8;

  if ((whiteBishops | blackBishops) & startTile) {
    startDir = 4;
  } else if ((whiteRooks | blackRooks) & startTile) {
    endDir = 4;
  }

  for (int i = startDir; i < endDir; i++) {

    if (endTiles[i] & startTile) {
      continue;
    }
    uint64_t endTile = (directions[i] > 0 ? startTile << directions[i]
                                          : startTile >> -directions[i]);
    while (1) {
      if ((color ? whitePieces : blackPieces) & endTile) {
        break;
      }
      moves->push_back(
          Move(startTile, endTile, findPiece(startTile), findPiece(endTile)));
      if (((color ? blackPieces : whitePieces) & endTile) ||
          endTiles[i] & endTile) {
        break;
      }
      if (directions[i] > 0) {
        endTile <<= directions[i];
      } else {
        endTile >>= -directions[i];
      }
    }
  }
}

void generateKingMoves(std::vector<Move> *moves, bool color,
                       uint64_t startTile) {}

std::vector<Move> generateMoves(bool color) {
  std::vector<Move> moves;

  for (int r = 7; r >= 0; r--) {
    for (int c = 7; c >= 0; c--) {

      whitePieces = (whitePawns | whiteKnights | whiteBishops | whiteRooks |
                     whiteQueens | whiteKings);
      blackPieces = (blackPawns | blackKnights | blackBishops | blackRooks |
                     blackQueens | blackKings);

      uint64_t tile = (1ULL << c) << (r * 8);

      if ((whitePawns & tile) && color) {
        generatePawnMoves(&moves, color, tile);
      } else if ((blackPawns & tile) && !color) {
        generatePawnMoves(&moves, color, tile);
      }

      if ((whiteKnights & tile) && color) {
        generateKnightMoves(&moves, color, tile);
      } else if ((blackKnights & tile) && !color) {
        generateKnightMoves(&moves, color, tile);
      }

      if (((whiteBishops | whiteRooks | whiteQueens) & tile) && color) {
        generateSlidingMoves(&moves, color, tile);
      } else if (((blackBishops | blackRooks | blackQueens) & tile) && !color) {
        generateSlidingMoves(&moves, color, tile);
      }

      if ((whiteKings & tile) && color) {
        generateKingMoves(&moves, color, tile);
      } else if ((blackKings & tile) && !color) {
        generateKingMoves(&moves, color, tile);
      }
    }
  }

  return moves;
}

void playMove(Move move) {
  *(move.movedPiece) ^= move.from;
  if (move.capturedPiece) {
    *(move.capturedPiece) ^= move.to;
  }
  *(move.movedPiece) ^= move.to;
}

int minimax(int depth, int alpha, int beta, bool maximizingPlayer) {
  if (depth == 0) {
    return getScore();
  }
  if (maximizingPlayer) {
    int maxEval = -INT_MAX;
    std::vector<Move> moves = generateMoves(maximizingPlayer);
    for (int i = 0; i < moves.size(); i++) {
      playMove(moves[i]);
      int eval = minimax(depth - 1, alpha, beta, 0);
      playMove(moves[i]);
      maxEval = std::max(maxEval, eval);
      alpha = std::max(alpha, eval);
      if (beta <= alpha) {
        break;
      }
    }
    return maxEval;
  } else {
    int minEval = INT_MAX;
    std::vector<Move> moves = generateMoves(maximizingPlayer);
    for (int i = 0; i < moves.size(); i++) {
      playMove(moves[i]);
      int eval = minimax(depth - 1, alpha, beta, 1);
      playMove(moves[i]);
      minEval = std::min(minEval, eval);
      beta = std::min(beta, eval);
      if (beta <= alpha) {
        break;
      }
    }
    return minEval;
  }
}

int main() {
  printBoard();
  std::cout << generateMoves(0).size();
  return 0;
}
