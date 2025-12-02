#include "board.h"

board::board() {
    resetBoard();
}

void board::resetBoard() {
    isWhiteTurn = true;
    castleRights = 0b1111;
    hasEnPassant = false;
    enPassantSquare = -1;
    halfmoveClock = 0;
    fullmoveNumber = 0;

    Piece init[64] = { BR, BN, BB, BQ, BK, BB, BN, BR,
                   BP, BP, BP, BP, BP, BP, BP, BP,
                   EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                   EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                   EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                   EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                   WP, WP, WP, WP, WP, WP, WP, WP,
                   WR, WN, WB, WQ, WK, WB, WN, WR };

    for (int i = 0; i < 64; i++) {
        currentState[i] = init[i];
    }

}
Unmove board::makeMove(const Move& move) {
    Unmove u;

    u.fromPiece = currentState[move.from];
    u.toPiece = currentState[move.to];

    u.prevCastleRights = castleRights;
    u.prevHasEnPassant = hasEnPassant;
    u.prevEnPassantSquare = enPassantSquare;

    u.prevTurn = isWhiteTurn;

    u.epCapturedPiece = EMPTY;
    u.epCapturedSquare = -1;

    // ---- Promotion ----
    if (move.wasPromotion) {
        currentState[move.from] = EMPTY;
        currentState[move.to] = move.promotedTo;

        castleRights = move.castleRights;
        hasEnPassant = move.hasEnPassant;
        enPassantSquare = move.enPassantSquare;

        isWhiteTurn = !isWhiteTurn;

        return u; // << FIXED
    }

    // ---- En Passant ----
    if (move.wasEnPassant) {
        if (move.moved == WP) {
            u.epCapturedPiece = BP;
            u.epCapturedSquare = move.to + 8;
            currentState[move.to + 8] = EMPTY;
        }
        else {
            u.epCapturedPiece = WP;
            u.epCapturedSquare = move.to - 8;
            currentState[move.to - 8] = EMPTY;
        }
    }

    // ---- Normal piece move ----
    currentState[move.from] = EMPTY;
    currentState[move.to] = move.moved;

    // ---- Castling ----
    if (move.wasCastling) {
        if (move.to == 6) {
            currentState[5] = BR;
            currentState[7] = EMPTY;
        }
        else if (move.to == 2) {
            currentState[3] = BR;
            currentState[0] = EMPTY;
        }
        else if (move.to == 58) {
            currentState[59] = WR;
            currentState[56] = EMPTY;
        }
        else if (move.to == 62) {
            currentState[61] = WR;
            currentState[63] = EMPTY;
        }
    }

    // ---- Update state ----
    hasEnPassant = move.hasEnPassant;
    enPassantSquare = move.enPassantSquare;
    castleRights = move.castleRights;
    isWhiteTurn = !isWhiteTurn;

    return u;
}

void board::unmakeMove(const Move& m, const Unmove& u) {

    // restore turn
    isWhiteTurn = u.prevTurn;

    // restore state
    castleRights = u.prevCastleRights;
    hasEnPassant = u.prevHasEnPassant;
    enPassantSquare = u.prevEnPassantSquare;

    // ---- Promotion ----
    if (m.wasPromotion) {
        // restore pawn
        currentState[m.from] = m.moved;
        // restore whatever was on target square
        currentState[m.to] = u.toPiece;
        return; // << FIXED
    }

    // restore normal pieces
    currentState[m.from] = u.fromPiece;
    currentState[m.to] = u.toPiece;

    // ---- Undo castling ----
    if (m.wasCastling) {
        if (m.to == 62) { // white O-O
            currentState[63] = WR;
            currentState[61] = EMPTY;
        }
        else if (m.to == 58) {
            currentState[56] = WR;
            currentState[59] = EMPTY;
        }
        else if (m.to == 6) {
            currentState[7] = BR;
            currentState[5] = EMPTY;
        }
        else if (m.to == 2) {
            currentState[0] = BR;
            currentState[3] = EMPTY;
        }
    }

    // ---- Undo en passant ----
    if (u.epCapturedPiece != EMPTY) {
        currentState[u.epCapturedSquare] = u.epCapturedPiece;
    }
}



void board::loadFEN(const std::string& fen) {
    // Reset the board to the initial state
    for (int i = 0; i < 64; i++) {
        currentState[i] = EMPTY;
    }


    // FEN format parts: piecePlacement, activeColor, castleRights, enPassant, halfmoveClock, fullmoveNumber
    size_t firstSpace = fen.find(' ');
    std::string piecePlacement = fen.substr(0, firstSpace);

    size_t secondSpace = fen.find(' ', firstSpace + 1);
    std::string activeColor = fen.substr(firstSpace + 1, secondSpace - firstSpace - 1);

    size_t thirdSpace = fen.find(' ', secondSpace + 1);
    std::string castleRightsstr = fen.substr(secondSpace + 1, thirdSpace - secondSpace - 1);

    size_t fourthSpace = fen.find(' ', thirdSpace + 1);
    std::string enPassant = fen.substr(thirdSpace + 1, fourthSpace - thirdSpace - 1);

    size_t fifthSpace = fen.find(' ', fourthSpace + 1);
    std::string halfmoveClockstr = fen.substr(fourthSpace + 1, fifthSpace - fourthSpace - 1);

    std::string fullmoveNumberstr = fen.substr(fifthSpace + 1);

    // Parse piece placement
    int index = 0;
    for (char c : piecePlacement) {
        if (c == '/') {
            continue;  // Skip the row separator
        }
        if (isdigit(c)) {
            // Skip empty squares (number indicates how many)
            index += c - '0';
        }
        else {
            // Map the piece character to the corresponding piece
            switch (c) {
            case 'r': currentState[index] = BR; break;
            case 'n': currentState[index] = BN; break;
            case 'b': currentState[index] = BB; break;
            case 'q': currentState[index] = BQ; break;
            case 'k': currentState[index] = BK; break;
            case 'p': currentState[index] = BP; break;
            case 'R': currentState[index] = WR; break;
            case 'N': currentState[index] = WN; break;
            case 'B': currentState[index] = WB; break;
            case 'Q': currentState[index] = WQ; break;
            case 'K': currentState[index] = WK; break;
            case 'P': currentState[index] = WP; break;
            default: throw std::invalid_argument("Invalid character in FEN.");
            }
            ++index;
        }
    }

    // Parse active color
    isWhiteTurn = (activeColor == "w");

    // Parse castling rights
    castleRights = 0;  // Initialize the bitmask
    if (castleRightsstr.find('K') != std::string::npos) castleRights |= 1;  // White kingside
    if (castleRightsstr.find('Q') != std::string::npos) castleRights |= 2;  // White queenside
    if (castleRightsstr.find('k') != std::string::npos) castleRights |= 4;  // Black kingside
    if (castleRightsstr.find('q') != std::string::npos) castleRights |= 8;  // Black queenside

    // Parse en passant
    if (enPassant != "-") {
        enPassantSquare = toIndex(enPassant[1] - '1', enPassant[0] - 'a');
        hasEnPassant = true;
    }
    else {
        hasEnPassant = false;
        enPassantSquare = -1;
    }

    // Parse halfmove clock and fullmove number
    halfmoveClock = std::stoi(halfmoveClockstr);
    fullmoveNumber = std::stoi(fullmoveNumberstr);
}



