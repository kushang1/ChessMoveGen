#include "movegenerator.h"

static constexpr int KNIGHT_DELTAS[8][2] = {
    { 2, 1}, { 2,-1}, {-2, 1}, {-2,-1},
    { 1, 2}, { 1,-2}, {-1, 2}, {-1,-2}
};
static constexpr int ROOK_DIRS[4][2] = {
    {1, 0}, {-1, 0}, {0, 1}, {0, -1}
};
static constexpr int BISHOP_DIRS[4][2] = {
    {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
};
static constexpr int QUEEN_DIRS[8][2] = {
    {1, 0}, {-1, 0}, {0, 1}, {0, -1},
    {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
};
static constexpr int KING_DIRS[8][2] = {
    {1, 0}, {-1, 0}, {0, 1}, {0, -1},
    {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
};
static constexpr Piece WHITE_PROMOS[4] = { WQ, WR, WB, WN };
static constexpr Piece BLACK_PROMOS[4] = { BQ, BR, BB, BN };



MoveGenerator::MoveGenerator() {

}

std::vector<Move> MoveGenerator::generatePseudoLegalMoves(board& Board) {
    std::vector<Move> moves;
    moves.reserve(96);

    // Iterate over all squares on the board
    for (int i = 0; i < 64; ++i) {
        Piece piece = Board.currentState[i];

        // Handle white pieces
        if (Board.isWhiteTurn) {
            switch (piece) {
            case WN:  // White Knight
                generateKnightMoves(Board, i, WN, moves);
                break;
            case WP:  // White Pawn
                generatePawnMoves(Board, i, WP, moves);
                break;
            case WR:  // White Rook
                generateSlidingMoves(Board, i, WR, moves);
                break;
            case WB:  // White Bishop
                generateSlidingMoves(Board, i, WB, moves);
                break;
            case WQ:  // White Queen
                generateSlidingMoves(Board, i, WQ, moves);
                break;
            case WK:  // White King
                generateKingMoves(Board, i, WK, moves);
                generateCastlingMoves(Board, i, WK, moves);
                break;
            default:
                break;
            }
        }
        // Handle black pieces
        else {
            switch (piece) {
            case BN:  // Black Knight
                generateKnightMoves(Board, i, BN, moves);
                break;
            case BP:  // Black Pawn
                generatePawnMoves(Board, i, BP, moves);
                break;
            case BR:  // Black Rook
                generateSlidingMoves(Board, i, BR, moves);
                break;
            case BB:  // Black Bishop
                generateSlidingMoves(Board, i, BB, moves);
                break;
            case BQ:  // Black Queen
                generateSlidingMoves(Board, i, BQ, moves);
                break;
            case BK:  // Black King
                generateKingMoves(Board, i, BK, moves);
                generateCastlingMoves(Board, i, BK, moves);
                break;
            default:
                break;
            }
        }
    }

    return moves;
}

// Knight move generation
void MoveGenerator::generateKnightMoves(board& Board, int i, Piece knightType, std::vector<Move>& moves) {
    

    int row = i / 8;
    int col = i % 8;

    // Loop through each possible knight move delta
    for (int k = 0; k < 8; ++k) {
        int newRow = row + KNIGHT_DELTAS[k][0];
        int newCol = col + KNIGHT_DELTAS[k][1];

        // Check if the new position is on the board (within 8x8 bounds)
        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
            int newIndex = newRow * 8 + newCol;
            Piece targetPiece = Board.currentState[newIndex];

            // If the square is empty, or contains an opponent's piece, it's a valid move
            if (targetPiece == EMPTY) {
                moves.emplace_back(i, newIndex, knightType, targetPiece, Board.castleRights);
            }
            else if ((isWhitePiece(knightType) && isBlackPiece(targetPiece)) ||
                (isBlackPiece(knightType) && isWhitePiece(targetPiece)))
            {
                if (targetPiece == WR && newIndex == 63) {
                    int castle = 0b1110;
                    castle = castle & Board.castleRights;
                    moves.emplace_back(i, newIndex, knightType, targetPiece, castle);
                }
                else if (targetPiece == WR && newIndex == 56) {
                    int castle = 0b1101;
                    castle = castle & Board.castleRights;
                    moves.emplace_back(i, newIndex, knightType, targetPiece, castle);
                }
                else if (targetPiece == BR && newIndex == 0) {
                    int castle = 0b0111;
                    castle = castle & Board.castleRights;
                    moves.emplace_back(i, newIndex, knightType, targetPiece, castle);
                }
                else if (targetPiece == BR && newIndex == 7) {
                    int castle = 0b1011;
                    castle = castle & Board.castleRights;
                    moves.emplace_back(i, newIndex, knightType, targetPiece, castle);
                }
                else {
                    moves.emplace_back(i, newIndex, knightType, targetPiece, Board.castleRights);
                }
            }
        }
    }
}

// Function to generate moves for sliding pieces: rook, bishop, and queen
void MoveGenerator::generateSlidingMoves(board& Board, int i, Piece piece, std::vector<Move>& moves) {
    int row = i / 8;
    int col = i % 8;

    const int (*dirs)[2];
    int dirCount;

    if (piece == WR || piece == BR) {
        dirs = ROOK_DIRS;
        dirCount = 4;
    }
    else if (piece == WB || piece == BB) {
        dirs = BISHOP_DIRS;
        dirCount = 4;
    }
    else { // queen
        dirs = QUEEN_DIRS;
        dirCount = 8;
    }

    // Loop through all directions
    for (int d = 0; d < dirCount; ++d) {
        int dr = dirs[d][0];
        int dc = dirs[d][1];
        int nr = row + dr, nc = col + dc;

        // Loop in the direction until hitting the edge of the board or a blockage
        while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
            int newIndex = nr * 8 + nc;
            Piece targetPiece = Board.currentState[newIndex];

            // If the square is empty, it's a valid move
            if (targetPiece == EMPTY) {
                if (piece != WR && piece != BR) {
                    moves.emplace_back(i, newIndex, piece, targetPiece, Board.castleRights);
                }
                else if (piece == WR) {
                    if (i == 63) {
                        int castle = 0b1110;
                        castle = castle & Board.castleRights;
                        moves.emplace_back(i, newIndex, piece, targetPiece, castle);
                    }
                    else if (i == 56) {
                        int castle = 0b1101;
                        castle = castle & Board.castleRights;
                        moves.emplace_back(i, newIndex, piece, targetPiece, castle);
                    }
                    else {
                        moves.emplace_back(i, newIndex, piece, targetPiece, Board.castleRights);
                    }
                }
                else if (piece == BR) {
                    if (i == 0) {
                        int castle = 0b0111;
                        castle = castle & Board.castleRights;
                        moves.emplace_back(i, newIndex, piece, targetPiece, castle);
                    }
                    else if (i == 7) {
                        int castle = 0b1011;
                        castle = castle & Board.castleRights;
                        moves.emplace_back(i, newIndex, piece, targetPiece, castle);
                    }
                    else {
                        moves.emplace_back(i, newIndex, piece, targetPiece, Board.castleRights);
                    }
                }
            }
            // If it's an opponent's piece, we can capture it
            else if ((isWhitePiece(piece) && isBlackPiece(targetPiece)) ||
                (isBlackPiece(piece) && isWhitePiece(targetPiece)))
            {

                if (piece != WR && piece != BR) {
                    if (targetPiece == WR && newIndex == 63) {
                        int castle = 0b1110;
                        castle = castle & Board.castleRights;
                        moves.emplace_back(i, newIndex, piece, targetPiece, castle);
                    }
                    else if (targetPiece == WR && newIndex == 56) {
                        int castle = 0b1101;
                        castle = castle & Board.castleRights;
                        moves.emplace_back(i, newIndex, piece, targetPiece, castle);
                    }
                    else if (targetPiece == BR && newIndex == 0) {
                        int castle = 0b0111;
                        castle = castle & Board.castleRights;
                        moves.emplace_back(i, newIndex, piece, targetPiece, castle);
                    }
                    else if (targetPiece == BR && newIndex == 7) {
                        int castle = 0b1011;
                        castle = castle & Board.castleRights;
                        moves.emplace_back(i, newIndex, piece, targetPiece, castle);
                    }
                    else {
                        moves.emplace_back(i, newIndex, piece, targetPiece, Board.castleRights);
                    }
                }
                else if (piece == WR) {
                    if (i == 63) {
                        int castle = 0b1110;
                        castle = castle & Board.castleRights;

                        if (targetPiece == BR && newIndex == 0) {
                            int castle1 = 0b0111;
                            castle1 = castle1 & castle;
                            moves.emplace_back(i, newIndex, piece, targetPiece, castle1);
                        }
                        else if (targetPiece == BR && newIndex == 7) {
                            int castle1 = 0b1011;
                            castle1 = castle1 & castle;
                            moves.emplace_back(i, newIndex, piece, targetPiece, castle1);
                        }
                        else {
                            moves.emplace_back(i, newIndex, piece, targetPiece, castle);
                        }


                    }
                    else if (i == 56) {
                        int castle = 0b1101;
                        castle = castle & Board.castleRights;

                        if (targetPiece == BR && newIndex == 0) {
                            int castle1 = 0b0111;
                            castle1 = castle1 & castle;
                            moves.emplace_back(i, newIndex, piece, targetPiece, castle1);
                        }
                        else if (targetPiece == BR && newIndex == 7) {
                            int castle1 = 0b1011;
                            castle1 = castle1 & castle;
                            moves.emplace_back(i, newIndex, piece, targetPiece, castle1);
                        }
                        else {
                            moves.emplace_back(i, newIndex, piece, targetPiece, castle);
                        }
                    }
                    else {
                        if (targetPiece == BR && newIndex == 0) {
                            int castle = 0b0111;
                            castle = castle & Board.castleRights;
                            moves.emplace_back(i, newIndex, piece, targetPiece, castle);
                        }
                        else if (targetPiece == BR && newIndex == 7) {
                            int castle = 0b1011;
                            castle = castle & Board.castleRights;
                            moves.emplace_back(i, newIndex, piece, targetPiece, castle);
                        }
                        else {
                            moves.emplace_back(i, newIndex, piece, targetPiece, Board.castleRights);
                        }
                    }
                }
                else if (piece == BR) {
                    if (i == 0) {
                        int castle = 0b0111;
                        castle = castle & Board.castleRights;

                        if (targetPiece == WR && newIndex == 63) {
                            int castle1 = 0b1110;
                            castle1 = castle1 & castle;
                            moves.emplace_back(i, newIndex, piece, targetPiece, castle1);
                        }
                        else if (targetPiece == WR && newIndex == 56) {
                            int castle1 = 0b1101;
                            castle1 = castle1 & castle;
                            moves.emplace_back(i, newIndex, piece, targetPiece, castle1);
                        }
                        else {
                            moves.emplace_back(i, newIndex, piece, targetPiece, castle);
                        }
                    }
                    else if (i == 7) {
                        int castle = 0b1011;
                        castle = castle & Board.castleRights;

                        if (targetPiece == WR && newIndex == 63) {
                            int castle1 = 0b1110;
                            castle1 = castle1 & castle;
                            moves.emplace_back(i, newIndex, piece, targetPiece, castle1);
                        }
                        else if (targetPiece == WR && newIndex == 56) {
                            int castle1 = 0b1101;
                            castle1 = castle1 & castle;
                            moves.emplace_back(i, newIndex, piece, targetPiece, castle1);
                        }
                        else {
                            moves.emplace_back(i, newIndex, piece, targetPiece, castle);
                        }
                    }
                    else {
                        if (targetPiece == WR && newIndex == 63) {
                            int castle = 0b1110;
                            castle = castle & Board.castleRights;
                            moves.emplace_back(i, newIndex, piece, targetPiece, castle);
                        }
                        else if (targetPiece == WR && newIndex == 56) {
                            int castle = 0b1101;
                            castle = castle & Board.castleRights;
                            moves.emplace_back(i, newIndex, piece, targetPiece, castle);
                        }
                        else {
                            moves.emplace_back(i, newIndex, piece, targetPiece, Board.castleRights);
                        }
                    }
                }
                break;
            }
            // Blocked by own piece or edge of the board, stop sliding
            else {
                break;
            }

            // Move in the same direction
            nr += dr;
            nc += dc;
        }
    }
}

void MoveGenerator::generateKingMoves(board& Board, int i, Piece kingType, std::vector<Move>& moves) {
    int row = i / 8;
    int col = i % 8;
    for (int d = 0; d < 8; ++d) {
        int nr = row + KING_DIRS[d][0];
        int nc = col + KING_DIRS[d][1];

        if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
            int newIndex = nr * 8 + nc;
            Piece targetPiece = Board.currentState[newIndex];

            if (targetPiece == EMPTY) {
                if (kingType == WK) {
                    int castle = 0b1100;
                    castle = castle & Board.castleRights;
                    moves.emplace_back(i, newIndex, kingType, EMPTY, castle);
                }
                else {
                    int castle = 0b0011;
                    castle = castle & Board.castleRights;
                    moves.emplace_back(i, newIndex, kingType, EMPTY, castle);
                }
            }
            if (kingType == WK && Board.isBlackPiece(targetPiece)) {
                int castle = 0b1100;
                castle = castle & Board.castleRights;
                if (targetPiece == BR && newIndex == 0) {
                    int castle1 = 0b0111;
                    castle1 = castle1 & castle;
                    moves.emplace_back(i, newIndex, kingType, targetPiece, castle1);
                }
                else if (targetPiece == BR && newIndex == 7) {
                    int castle1 = 0b1011;
                    castle1 = castle1 & castle;
                    moves.emplace_back(i, newIndex, kingType, targetPiece, castle1);
                }
                else {
                    moves.emplace_back(i, newIndex, kingType, targetPiece, castle);
                }
            }
            else if (kingType == BK && isWhitePiece(targetPiece)) {
                int castle = 0b0011;
                castle = castle & Board.castleRights;
                if (targetPiece == WR && newIndex == 63) {
                    int castle1 = 0b1110;
                    castle1 = castle1 & castle;
                    moves.emplace_back(i, newIndex, kingType, targetPiece, castle1);
                }
                else if (targetPiece == WR && newIndex == 56) {
                    int castle1 = 0b1101;
                    castle1 = castle1 & castle;
                    moves.emplace_back(i, newIndex, kingType, targetPiece, castle1);
                }
                else {
                    moves.emplace_back(i, newIndex, kingType, targetPiece, castle);
                }
            }
        }
    }
}

void MoveGenerator::generateCastlingMoves(board& Board, int i, Piece kingType, std::vector<Move>& moves) {

    if (kingType == WK) {
        if ((Board.castleRights & 0b0011) == 0) {
            return;
        }
        if ((Board.castleRights & 0b0001) == 0b0001) {
            if (Board.currentState[i + 1] == EMPTY && Board.currentState[i + 2] == EMPTY) {
                if (Board.currentState[i + 3] == WR) {
                    int castle = 0b1100;
                    castle = castle & Board.castleRights;
                    moves.emplace_back(i, i + 2, kingType, EMPTY, castle);
                    moves.back().wasCastling = true;
                }
            }
        }
        if ((Board.castleRights & 0b0010) == 0b0010) {
            if (Board.currentState[i - 1] == EMPTY && Board.currentState[i - 2] == EMPTY && Board.currentState[i - 3] == EMPTY) {
                if (Board.currentState[i - 4] == WR) {
                    int castle = 0b1100;
                    castle = castle & Board.castleRights;
                    moves.emplace_back(i, i - 2, kingType, EMPTY, castle);
                    moves.back().wasCastling = true;
                }
            }
        }
    }
    else if (kingType == BK) {
        if ((Board.castleRights & 0b1100) == 0) {
            return;
        }
        if ((Board.castleRights & 0b0100) == 0b0100) {
            if (Board.currentState[i + 1] == EMPTY && Board.currentState[i + 2] == EMPTY) {
                if (Board.currentState[i + 3] == BR) {
                    int castle = 0b0011;
                    castle = castle & Board.castleRights;
                    moves.emplace_back(i, i + 2, kingType, EMPTY, castle);
                    moves.back().wasCastling = true;
                }
            }
        }
        if ((Board.castleRights & 0b1000) == 0b1000) {
            if (Board.currentState[i - 1] == EMPTY && Board.currentState[i - 2] == EMPTY && Board.currentState[i - 3] == EMPTY) {
                if (Board.currentState[i - 4] == BR) {
                    int castle = 0b0011;
                    castle = castle & Board.castleRights;
                    moves.emplace_back(i, i - 2, kingType, EMPTY, castle);
                    moves.back().wasCastling = true;
                }
            }
        }
    }
}

void MoveGenerator::generatePawnMoves(board& Board, int i, Piece pawnType, std::vector<Move>& moves)
{
    int row = i / 8;
    int col = i % 8;

    int forward = (pawnType == WP ? -1 : 1);
    int startRank = (pawnType == WP ? 6 : 1);
    int promoRank = (pawnType == WP ? 1 : 6);

    Piece enemyPawn = (pawnType == WP ? BP : WP);

    // -------------------------------
    // 1. PROMOTION REGION
    // -------------------------------
    if (row == promoRank)
    {
        int oneForward = Board.toIndex(row + forward, col);

        // forward promotion
        if (Board.currentState[oneForward] == EMPTY)
        {
            const Piece* promo = (pawnType == WP ? WHITE_PROMOS : BLACK_PROMOS);
            for (int k = 0; k < 4; ++k)
            {
                moves.emplace_back(i, oneForward, pawnType, (Piece)EMPTY, Board.castleRights);
                moves.back().wasPromotion = true;
                moves.back().promotedTo = promo[k];
            }
        }

        // capture promotions
        int capCols[2] = { col - 1, col + 1 };
        for (int c = 0; c < 2; ++c)
        {
            int cc = capCols[c];
            if (cc < 0 || cc > 7) continue;

            int capIndex = Board.toIndex(row + forward, cc);
            Piece target = Board.currentState[capIndex];
            if (target == EMPTY) continue;

            if (isWhitePiece(pawnType) ? isBlackPiece(target)
                : isWhitePiece(target))
            {
                int castleAfter = Board.castleRights;

                // rook capture → adjust castle rights
                if (target == WR && capIndex == 63) castleAfter &= 0b1110;
                if (target == WR && capIndex == 56) castleAfter &= 0b1101;
                if (target == BR && capIndex == 0)  castleAfter &= 0b0111;
                if (target == BR && capIndex == 7)  castleAfter &= 0b1011;

                const Piece* promo = (pawnType == WP ? WHITE_PROMOS : BLACK_PROMOS);
                for (int k = 0; k < 4; ++k)
                {
                    moves.emplace_back(i, capIndex, pawnType, target, castleAfter);
                    moves.back().wasPromotion = true;
                    moves.back().promotedTo = promo[k];
                }
            }
        }

        return;
    }

    // -------------------------------
    // 2. NORMAL (NON-PROMO) MOVES
    // -------------------------------

    // forward one
    int oneForward = toIndex(row + forward, col);
    if (Board.currentState[oneForward] == EMPTY)
    {
        moves.emplace_back(i, oneForward, pawnType, (Piece)EMPTY, Board.castleRights);

        // forward two
        if (row == startRank)
        {
            int twoForward = toIndex(row + forward * 2, col);
            if (Board.currentState[twoForward] == EMPTY)
            {
                moves.emplace_back(i, twoForward, pawnType, (Piece)EMPTY, Board.castleRights);

                // en-passant marking
                int leftCol = col - 1;
                int rightCol = col + 1;

                if ((leftCol >= 0 && Board.currentState[toIndex(row + 2 * forward, leftCol)] == enemyPawn) ||
                    (rightCol <= 7 && Board.currentState[toIndex(row + 2 * forward, rightCol)] == enemyPawn))
                {
                    moves.back().hasEnPassant = true;
                    moves.back().enPassantSquare = oneForward;
                }
            }
        }
    }

    // -------------------------------
    // 3. NORMAL CAPTURES
    // -------------------------------
    int capCols[2] = { col - 1, col + 1 };
    for (int c = 0; c < 2; ++c)
    {
        int cc = capCols[c];
        if (cc < 0 || cc > 7) continue;

        int capIndex = toIndex(row + forward, cc);
        Piece target = Board.currentState[capIndex];

        if (target == EMPTY) continue;

        if (isWhitePiece(pawnType) ? isBlackPiece(target)
            : isWhitePiece(target))
        {
            int castleAfter = Board.castleRights;

            if (target == WR && capIndex == 63) castleAfter &= 0b1110;
            if (target == WR && capIndex == 56) castleAfter &= 0b1101;
            if (target == BR && capIndex == 0)  castleAfter &= 0b0111;
            if (target == BR && capIndex == 7)  castleAfter &= 0b1011;

            moves.emplace_back(i, capIndex, pawnType, target, castleAfter);
        }
    }

    // -------------------------------
    // 4. EN PASSANT CAPTURE
    // -------------------------------
    if (Board.hasEnPassant)
    {
        int ep = Board.enPassantSquare;

        for (int cc : capCols)
        {
            if (cc < 0 || cc > 7) continue;

            int epIndex = toIndex(row + forward, cc);

            if (epIndex == ep)
            {
                moves.emplace_back(i, epIndex, pawnType, enemyPawn, Board.castleRights);
                moves.back().wasEnPassant = true;
            }
        }
    }
}


bool MoveGenerator::canCaptureKing(board& Board) {
    // side to move
    bool sideWhite = Board.isWhiteTurn;

    // find enemy king
    Piece enemyKing = sideWhite ? BK : WK;
    int kingSq = -1;
    for (int i = 0; i < 64; ++i) {
        if (Board.currentState[i] == enemyKing) {
            kingSq = i;
            break;
        }
    }
    if (kingSq == -1) return false; // should not happen

    // is the enemy king's square attacked by side to move?
    return isSquareAttacked(Board, kingSq, sideWhite);
}


bool MoveGenerator::canCastle(board& Board, const Move& move) {
    // Board.isWhiteTurn here is the *opponent* (you flipped before calling)
    bool enemyWhite = Board.isWhiteTurn;

    int from = move.from;
    int mid = -1;
    int to = move.to;

    if (to == 6)       mid = 5;   // black O-O: e8 -> g8
    else if (to == 2)  mid = 3;   // black O-O-O: e8 -> c8
    else if (to == 62) mid = 61;  // white O-O: e1 -> g1
    else if (to == 58) mid = 59;  // white O-O-O: e1 -> c1

    // King cannot be in check on from, through, or to squares
    if (isSquareAttacked(Board, from, enemyWhite)) return false;
    if (isSquareAttacked(Board, mid, enemyWhite)) return false;
    if (isSquareAttacked(Board, to, enemyWhite)) return false;

    return true;
}


std::vector<Move> MoveGenerator::generateLegalMoves(board& Board) {
    std::vector<Move> pseudoLegal = generatePseudoLegalMoves(Board);
    std::vector<Move> legal;
    legal.reserve(pseudoLegal.size());

    for (auto& i : pseudoLegal) {

        // --- Castling legality check ---
        if (i.wasCastling) {
            Board.isWhiteTurn = !Board.isWhiteTurn;
            bool ok = canCastle(Board, i);   // check
            Board.isWhiteTurn = !Board.isWhiteTurn;

            if (!ok) continue; // always restore turn before continuing
        }

        // --- Make move ---
        Unmove u = Board.makeMove(i);

        // --- Test for check ---
        if (!canCaptureKing(Board)) {
            legal.push_back(i);
        }

        // --- Undo move (correct order!) ---
        Board.unmakeMove(i, u);
    }

    return legal;
}



int MoveGenerator::findKing(const board& Board, bool white) {
    Piece king = white ? WK : BK;
    for (int i = 0; i < 64; ++i) {
        if (Board.currentState[i] == king) return i;
    }
    return -1; // should never happen
}

bool MoveGenerator::isSquareAttacked(const board& Board, int sq, bool byWhite) {
    int row = sq / 8;
    int col = sq % 8;

    // --- Pawn attacks ---
    if (byWhite) {
        int r = row + 1;
        if (r < 8) {
            if (col > 0 && Board.currentState[r * 8 + (col - 1)] == WP) return true;
            if (col < 7 && Board.currentState[r * 8 + (col + 1)] == WP) return true;
        }
    }
    else {
        int r = row - 1;
        if (r >= 0) {
            if (col > 0 && Board.currentState[r * 8 + (col - 1)] == BP) return true;
            if (col < 7 && Board.currentState[r * 8 + (col + 1)] == BP) return true;
        }
    }

    // --- Knight attacks ---
    static constexpr int knightDeltas[8][2] = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
        {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };
    for (auto& d : knightDeltas) {
        int nr = row + d[0];
        int nc = col + d[1];
        if (nr < 0 || nr >= 8 || nc < 0 || nc >= 8) continue;
        Piece p = Board.currentState[nr * 8 + nc];
        if (byWhite && p == WN) return true;
        if (!byWhite && p == BN) return true;
    }

    // --- King attacks (adjacent squares) ---
    static constexpr int kingDeltas[8][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };
    for (auto& d : kingDeltas) {
        int nr = row + d[0];
        int nc = col + d[1];
        if (nr < 0 || nr >= 8 || nc < 0 || nc >= 8) continue;
        Piece p = Board.currentState[nr * 8 + nc];
        if (byWhite && p == WK) return true;
        if (!byWhite && p == BK) return true;
    }

    // --- Sliding attacks: bishops/queens (diagonals) ---
    static constexpr int bishopDirs[4][2] = {
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };
    for (auto& d : bishopDirs) {
        int nr = row + d[0];
        int nc = col + d[1];
        while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
            Piece p = Board.currentState[nr * 8 + nc];
            if (p != EMPTY) {
                if (byWhite && (p == WB || p == WQ)) return true;
                if (!byWhite && (p == BB || p == BQ)) return true;
                break;
            }
            nr += d[0];
            nc += d[1];
        }
    }

    // --- Sliding attacks: rooks/queens (orthogonals) ---
    static constexpr int rookDirs[4][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };
    for (auto& d : rookDirs) {
        int nr = row + d[0];
        int nc = col + d[1];
        while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
            Piece p = Board.currentState[nr * 8 + nc];
            if (p != EMPTY) {
                if (byWhite && (p == WR || p == WQ)) return true;
                if (!byWhite && (p == BR || p == BQ)) return true;
                break;
            }
            nr += d[0];
            nc += d[1];
        }
    }

    return false;
}
