#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "board.h"

class MoveGenerator
{
public:
    MoveGenerator();
    std::vector<Move> generatePseudoLegalMoves(board& Board);
    std::vector<Move> generateLegalMoves(board& Board);


    void generateKnightMoves(board& Board, int i, Piece knightType, std::vector<Move>& moves);
    void generateSlidingMoves(board& Board, int i, Piece piece, std::vector<Move>& moves);
    void generateKingMoves(board& Board, int i, Piece kingType, std::vector<Move>& moves);
    void generateCastlingMoves(board& Board, int i, Piece kingType, std::vector<Move>& moves);
    void generatePawnMoves(board& Board, int i, Piece pawnType, std::vector<Move>& moves);

    bool canCaptureKing(board& Board);
    bool canCastle(board& Board, const Move& move);
    bool isSquareAttacked(const board& Board, int sq, bool byWhite);
    int findKing(const board& Board, bool white);
    
    inline int toIndex(int row, int col) const { return (row << 3) | col; }
    inline bool isWhitePiece(Piece p) {
        return p >= WQ && p <= WB;
    }

    inline bool isBlackPiece(Piece p) {
        return p >= BQ && p <= BB;
    }

};

#endif
