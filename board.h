#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <QDebug>

enum Piece {
    EMPTY,
    BQ, BR, BP, BN, BK, BB,
    WQ, WR, WP, WN, WK, WB
};

struct Unmove {
    Piece fromPiece;        // piece originally at move.from
    Piece toPiece;          // piece originally at move.to

    int prevCastleRights;   // full previous board castling rights
    bool prevHasEnPassant;
    int prevEnPassantSquare;

    bool prevTurn;          // who was to move before makeMove

    // For en-passant undo:
    Piece epCapturedPiece;
    int epCapturedSquare;    // square where it was removed
};


class Move {
public:
    Move() {}
    Move(int from, int to, Piece moved, Piece captured, int castleRights) {
        this->from = from;
        this->to = to;
        this->moved = moved;
        this->captured = captured;
        this->castleRights = castleRights;
    }
    int from;
    int to;
    Piece moved;
    Piece captured;

    bool wasCastling = false;
    int castleRights;

    bool wasPromotion = false;
    Piece promotedTo;

    bool wasEnPassant = false;

    bool hasEnPassant = false;
    int enPassantSquare = -1;
};

class board
{
public:
    board();
    void resetBoard();
    void loadFEN(const std::string& fen);
    Unmove makeMove(const Move& m);

    void unmakeMove(const Move& m, const Unmove& u);
    inline int toIndex(int row, int col) const { return (row << 3) | col; }
    inline bool isWhitePiece(Piece p) {
        return p >= WQ && p <= WB;
    }

    inline bool isBlackPiece(Piece p) {
        return p >= BQ && p <= BB;
    }
  

public:
    Piece currentState[64];
    bool isWhiteTurn;
    int castleRights;
    bool hasEnPassant;
    int enPassantSquare;
    int halfmoveClock;
    int fullmoveNumber;
  
};

#endif // BOARD_H
