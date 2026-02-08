#include <SFML/Graphics.hpp>
#include <climits>
#include <iostream>
#include <random>
#include <vector>

sf::RenderWindow window(sf::VideoMode({800, 825}), "Chess");
sf::RectangleShape lightTile({100, 100});
sf::RectangleShape darkTile({100, 100});
sf::RectangleShape statusBar({800, 25});

const sf::Texture whitePawnTexture("pieces/white-pawn.png");
const sf::Texture whiteKnightTexture("pieces/white-knight.png");
const sf::Texture whiteBishopTexture("pieces/white-bishop.png");
const sf::Texture whiteRookTexture("pieces/white-rook.png");
const sf::Texture whiteQueenTexture("pieces/white-queen.png");
const sf::Texture whiteKingTexture("pieces/white-king.png");

const sf::Texture blackPawnTexture("pieces/black-pawn.png");
const sf::Texture blackKnightTexture("pieces/black-knight.png");
const sf::Texture blackBishopTexture("pieces/black-bishop.png");
const sf::Texture blackRookTexture("pieces/black-rook.png");
const sf::Texture blackQueenTexture("pieces/black-queen.png");
const sf::Texture blackKingTexture("pieces/black-king.png");

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distr(1, 100);

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

const int MAX_DEPTH = 4;
const int MAX_MOVES = 1000;
const float TEMPERATURE = 1.5;
const int GAME_MODE = 0;

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

uint64_t from;
uint64_t to;
uint64_t *movedPiece;
uint64_t *capturedPiece;

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
  bool whiteKing = 0;
  bool blackKing = 0;

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
      } else if (whiteKings & tile) {
        whiteKing = 1;
      } else if (blackKings & tile) {
        blackKing = 1;
      }
    }
  }
  if (!whiteKing) {
    score = -INT_MAX;
  }
  if (!blackKing) {
    score = INT_MAX;
  }
  return score + (distr(gen) * TEMPERATURE);
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
          (endTiles[i] & endTile) || ((whiteKings | blackKings) & startTile)) {
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

      if (((whiteBishops | whiteRooks | whiteQueens | whiteKings) & tile) &&
          color) {
        generateSlidingMoves(&moves, color, tile);
      } else if (((blackBishops | blackRooks | blackQueens | blackKings) &
                  tile) &&
                 !color) {
        generateSlidingMoves(&moves, color, tile);
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

      if ((depth == MAX_DEPTH) && (eval > maxEval)) {
        from = moves[i].from;
        to = moves[i].to;
        movedPiece = moves[i].movedPiece;
        capturedPiece = moves[i].capturedPiece;
      }

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

      if ((depth == MAX_DEPTH) && (eval < minEval)) {
        from = moves[i].from;
        to = moves[i].to;
        movedPiece = moves[i].movedPiece;
        capturedPiece = moves[i].capturedPiece;
      }

      minEval = std::min(minEval, eval);
      beta = std::min(beta, eval);
      if (beta <= alpha) {
        break;
      }
    }
    return minEval;
  }
}

void renderBoard() {
  lightTile.setFillColor(sf::Color(238, 238, 210));
  darkTile.setFillColor(sf::Color(118, 150, 86));
  statusBar.setFillColor(sf::Color(255, 255, 255));
  window.clear();

  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 8; c++) {

      bool empty = 0;
      uint64_t tile = (1ULL << (7 - r)) << ((7 - c) * 8);
      sf::Texture tileTexture;

      if (whitePawns & tile) {
        tileTexture = whitePawnTexture;
      } else if (blackPawns & tile) {
        tileTexture = blackPawnTexture;
      } else if (whiteKnights & tile) {
        tileTexture = whiteKnightTexture;
      } else if (blackKnights & tile) {
        tileTexture = blackKnightTexture;
      } else if (whiteBishops & tile) {
        tileTexture = whiteBishopTexture;
      } else if (blackBishops & tile) {
        tileTexture = blackBishopTexture;
      } else if (whiteRooks & tile) {
        tileTexture = whiteRookTexture;
      } else if (blackRooks & tile) {
        tileTexture = blackRookTexture;
      } else if (whiteQueens & tile) {
        tileTexture = whiteQueenTexture;
      } else if (blackQueens & tile) {
        tileTexture = blackQueenTexture;
      } else if (whiteKings & tile) {
        tileTexture = whiteKingTexture;
      } else if (blackKings & tile) {
        tileTexture = blackKingTexture;
      } else {
        empty = 1;
      }

      if ((r + c) & 1) {
        lightTile.setPosition({(float)r * 100, (float)c * 100});
        window.draw(lightTile);

      } else {
        darkTile.setPosition({(float)r * 100, (float)c * 100});
        window.draw(darkTile);
      }

      if (!empty) {
        sf::Sprite tileSprite(tileTexture);
        tileSprite.setPosition({(float)r * 100, (float)c * 100});
        tileSprite.setScale({0.78125, 0.78125});
        window.draw(tileSprite);
      }
    }
  }
  statusBar.setPosition({((float)getScore() / 1) - 400, 800});
  window.draw(statusBar);
  window.display();
}

int main() {

  renderBoard();

  bool secondClick = 0;
  int moveCount = 0;
  while (window.isOpen()) {

    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>())
        window.close();

      if (event->is<sf::Event::MouseButtonPressed>() &&
          (GAME_MODE == 0 && (moveCount % 2 == 0))) {

        int r = (int)(7 - std::floor(sf::Mouse::getPosition(window).y / 100));
        int c = (int)(7 - std::floor(sf::Mouse::getPosition(window).x / 100));

        if (secondClick) {
          to = (1ULL << 8 * r) << c;
          capturedPiece = findPiece(to);
          std::vector<Move> legalMoves = generateMoves(1);
          Move nextMove = Move(from, to, movedPiece, capturedPiece);
          for (int i = 0; i < legalMoves.size(); i++) {
            if (legalMoves[i].from == nextMove.from &&
                legalMoves[i].to == nextMove.to) {
              playMove(nextMove);
              moveCount += 1;
              renderBoard();
            }
          }

        } else {
          from = (1ULL << 8 * r) << c;
          movedPiece = findPiece(from);
        }
        secondClick = !secondClick;
      }
    }

    if (moveCount <= MAX_MOVES && GAME_MODE == 1 ||
        (GAME_MODE == 0 && (moveCount % 2 == 1))) {
      minimax(MAX_DEPTH, -INT_MAX, INT_MAX, moveCount % 2 == 0);
      Move nextMove = Move(from, to, movedPiece, capturedPiece);
      playMove(nextMove);
      moveCount += 1;
      renderBoard();
    }
  }
  return 0;
}
