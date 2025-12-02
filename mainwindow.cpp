#include "mainwindow.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    // --- Main horizontal layout ---
    QHBoxLayout* mainLayout = new QHBoxLayout(central);

    // ================================================================
    // LEFT PANEL
    // ================================================================
    QVBoxLayout* leftPanel = new QVBoxLayout();

    // Give the left panel inner padding so children are not flush to the edge
    leftPanel->setContentsMargins(8, 8, 8, 8);
    leftPanel->setSpacing(6);

    QWidget* leftWidget = new QWidget();
    leftWidget->setLayout(leftPanel);
    leftWidget->setFixedWidth(150);
    mainLayout->addWidget(leftWidget);

    // Undo/Redo buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* undoBtn = new QPushButton("Undo", this);
    QPushButton* redoBtn = new QPushButton("Redo", this);
    undoBtn->setFixedSize(50, 30);
    redoBtn->setFixedSize(50, 30);
    undoBtn->setStyleSheet("font-weight: bold; font-size: 14px;");
    redoBtn->setStyleSheet("font-weight: bold; font-size: 14px;");

    buttonLayout->addWidget(undoBtn);
    buttonLayout->addWidget(redoBtn);

    // Put the buttons at the top-left of the left panel (keep them left aligned)
    leftPanel->addLayout(buttonLayout);

    // Move history label (left aligned inside left panel)
    QLabel* historyLabel = new QLabel("Move History", this);
    historyLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    historyLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    leftPanel->addWidget(historyLabel);

    // Move history list - give it a clear frame and a fixed width that fits inside left panel margins
    moveHistoryList = new QListWidget(this);
    moveHistoryList->setFixedWidth(134); // 120 - left/right margins (8+8) = 104
    moveHistoryList->setStyleSheet(
        "font-size: 13px; padding-left: 6px; padding-top: 4px; padding-bottom: 4px;"
    );
    moveHistoryList->setFrameShape(QFrame::StyledPanel);
    moveHistoryList->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    // Center the list horizontally inside the left panel but leave it touching top after label
    QHBoxLayout* historyCenter = new QHBoxLayout();
    historyCenter->setContentsMargins(0, 0, 0, 0);
    historyCenter->addStretch();
    historyCenter->addWidget(moveHistoryList);
    historyCenter->addStretch();
    leftPanel->addLayout(historyCenter);

    leftPanel->addStretch();

    connect(undoBtn, &QPushButton::clicked, this, &MainWindow::undoMove);
    connect(redoBtn, &QPushButton::clicked, this, &MainWindow::redoMove);

    // ================================================================
    // CENTER PANEL (turn label + board with rank/file labels)
    // ================================================================
    QVBoxLayout* centerPanel = new QVBoxLayout();
    mainLayout->addLayout(centerPanel);

    // Turn label
    turnLabel = new QLabel("White's Turn", this);
    turnLabel->setAlignment(Qt::AlignCenter);
    turnLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    centerPanel->addWidget(turnLabel, 0, Qt::AlignHCenter);

    // Outer wrapper for board + labels
    QWidget* boardWithLabels = new QWidget(this);
    QGridLayout* outerGrid = new QGridLayout(boardWithLabels);
    outerGrid->setSpacing(0);
    outerGrid->setContentsMargins(0, 0, 0, 0);

    // --- Rank labels (1–8) on left ---
    for (int r = 0; r < 8; r++) {
        QLabel* rank = new QLabel(QString::number(8 - r));
        rank->setAlignment(Qt::AlignCenter);
        rank->setStyleSheet("font-size: 14px; font-weight: bold;");
        rank->setFixedSize(20, 80);
        outerGrid->addWidget(rank, r, 0);
    }

    // --- File labels (A–H) on bottom ---
    for (int c = 0; c < 8; c++) {
        QLabel* file = new QLabel(QString(QChar('A' + c)));
        file->setAlignment(Qt::AlignCenter);
        file->setStyleSheet("font-size: 14px; font-weight: bold;");
        file->setFixedSize(80, 20);
        outerGrid->addWidget(file, 8, c + 1);
    }

    // --- Actual chessboard ---
    QWidget* boardWidget = new QWidget(this);
    boardWidget->setFixedSize(640, 640);

    QGridLayout* grid = new QGridLayout(boardWidget);
    grid->setSpacing(0);
    grid->setContentsMargins(0, 0, 0, 0);

    const int SIZE = 8;
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {

            QPushButton* tile = new QPushButton(this);
            QString color = ((row + col) % 2 == 0) ? "#EEEED2" : "#769656";
            tile->setFixedSize(80, 80);

            // FULL square chess-piece behavior (chess.com-like)
            tile->setIconSize(QSize(80, 80));
            tile->setStyleSheet(
                "background-color:" + color +
                "; border: none; padding: 0px; margin: 0px;"
            );
            grid->addWidget(tile, row, col);
            boardButtons[row][col] = tile;

            connect(tile, &QPushButton::clicked, this, [=]() {
                selectedSquare = row * 8 + col;
                handleTileClick();
                });
        }
    }

    // Add board inside outer grid (row 0–7, col 1–8)
    outerGrid->addWidget(boardWidget, 0, 1, 8, 8);

    centerPanel->addWidget(boardWithLabels, 0, Qt::AlignHCenter);
    centerPanel->addStretch();

    // ================================================================
    // RIGHT PANEL
    // ================================================================
    QVBoxLayout* rightPanel = new QVBoxLayout();
    QWidget* rightWidget = new QWidget();
    rightWidget->setLayout(rightPanel);
    rightWidget->setFixedWidth(120);
    mainLayout->addWidget(rightWidget);

    QPushButton* pushie = new QPushButton("Calculate Moves", this);
    pushie->setFixedSize(50, 30);
    pushie->setStyleSheet("font-weight: bold; font-size: 14px;");
    rightPanel->addWidget(pushie);

    connect(pushie, &QPushButton::clicked, this, &MainWindow::CalculateMoves);

    // ================================================================
    // Init game
    // ================================================================
    //gameBoard.resetBoard();
    const std::string fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    gameBoard.loadFEN(fen);
    updateBoardUI();

    // Keyboard shortcuts
    QShortcut* undoShortcut = new QShortcut(QKeySequence("Ctrl+Z"), this);
    connect(undoShortcut, &QShortcut::activated, this, &MainWindow::undoMove);

    QShortcut* redoShortcut = new QShortcut(QKeySequence("Ctrl+Y"), this);
    connect(redoShortcut, &QShortcut::activated, this, &MainWindow::redoMove);

    // engineWatcher = new QFutureWatcher<Move>(this);
    // connect(engineWatcher, &QFutureWatcher<Move>::finished, this, &MainWindow::onEngineMoveReady);
}

MainWindow::~MainWindow()
{
}

void MainWindow::handleTileClick() {

    static int fromRow = -1, fromCol = -1;

    if (!pieceSelected) {
        Piece piece = gameBoard.currentState[selectedSquare];
        if (piece == EMPTY) return;

        bool isWhitePiece = (piece >= WQ && piece <= WB);

        if ((gameBoard.isWhiteTurn && !isWhitePiece) || (!gameBoard.isWhiteTurn && isWhitePiece))
            return;

        pieceSelected = true;
        fromRow = selectedSquare / 8;
        fromCol = selectedSquare % 8;

        boardButtons[fromRow][fromCol]->setStyleSheet("background-color: yellow; border: none;");

        auto moves = moveGenerator.generateLegalMoves(gameBoard);
        std::vector<Move> mv;
        for (auto i : moves) {
            if (i.from == selectedSquare) {
                mv.push_back(i);;
            }
        }
        highlightMoves(mv);


        return;
    }

    if (pieceSelected) {
        bool valid = false;
        Move mv;
        clearHighlights();
        auto legalMoves = moveGenerator.generateLegalMoves(gameBoard);
        for (auto &i : legalMoves) {
            if (i.to == selectedSquare && i.from == gameBoard.toIndex(fromRow, fromCol)) {
                mv = i;
                valid = true;
                break;
            }
        }

        resetColors();

        if (valid) {
            gameBoard.makeMove(mv);
            updateBoardUI();
            
        }
        if (valid) {
            Move engineMove = engine.findBestMove(gameBoard, 2);
            gameBoard.makeMove(engineMove);
            updateBoardUI();
        }
        

        pieceSelected = false;
        fromRow = fromCol = -1;
    }

}

void MainWindow::undoMove() {

}

void MainWindow::redoMove() {

}

void MainWindow::updateBoardUI() {

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            int ind = gameBoard.toIndex(row, col);
            Piece piece = gameBoard.currentState[ind];
            QString iconPath;

            switch (piece) {
            case WP: iconPath = "images/white_pawn.png"; break;
            case WR: iconPath = "images/white_rook.png"; break;
            case WN: iconPath = "images/white_knight.png"; break;
            case WB: iconPath = "images/white_bishop.png"; break;
            case WQ: iconPath = "images/white_queen.png"; break;
            case WK: iconPath = "images/white_king.png"; break;

            case BP: iconPath = "images/black_pawn.png"; break;
            case BR: iconPath = "images/black_rook.png"; break;
            case BN: iconPath = "images/black_knight.png"; break;
            case BB: iconPath = "images/black_bishop.png"; break;
            case BQ: iconPath = "images/black_queen.png"; break;
            case BK: iconPath = "images/black_king.png"; break;

            default: iconPath = ""; break;
            }

            if (!iconPath.isEmpty()) {
                QIcon icon(iconPath);
                boardButtons[row][col]->setIcon(icon);
                boardButtons[row][col]->setIconSize(boardButtons[row][col]->size());
            }
            else {
                boardButtons[row][col]->setIcon(QIcon());
            }
        }
    }
}

void MainWindow::resetColors()
{
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            QString color = ((r + c) % 2 == 0) ? "#EEEED2" : "#769656";
            boardButtons[r][c]->setStyleSheet("background-color:" + color + "; border: none;");
        }
    }
}

void MainWindow::highlightMoves(std::vector<Move>& moves) {
    clearHighlights();

    for (auto& mv : moves) {
        int square = mv.to;
        int r = square / 8;
        int c = square % 8;
        QWidget* overlay = new QWidget(boardButtons[r][c]);
        overlay->setAttribute(Qt::WA_TransparentForMouseEvents);
        overlay->setGeometry(0, 0, 80, 80);

        bool isCapture = (gameBoard.currentState[square] != EMPTY);

        if (!isCapture) {

            QWidget* dot = new QWidget(overlay);
            dot->setAttribute(Qt::WA_TransparentForMouseEvents);

            dot->setFixedSize(22, 22);                 // perfect dot size
            dot->move((80 - 22) / 2, (80 - 22) / 2);   // perfectly centered

            dot->setStyleSheet(
                "background-color: rgba(0,0,0,50);"     // translucent black
                "border-radius: 11px;"                  // circle
                "border: none;"
            );

            dot->show();
        }
        else {
            // ---- Capture Move: Even translucent ring ----
            overlay->setStyleSheet("background-color: transparent;");
            overlay->setFixedSize(80, 80);
            overlay->move(0, 0);

            // Custom paint event for clean ring
            overlay->installEventFilter(this);
        }

        overlay->show();
        highlightOverlays.push_back(overlay);
    }
}

void MainWindow::clearHighlights() {
    for (auto* w : highlightOverlays) {
        w->deleteLater();
    }
    highlightOverlays.clear();
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::Paint) {
        QWidget* w = qobject_cast<QWidget*>(obj);
        if (!w) return false;

        QPainter p(w);
        p.setRenderHint(QPainter::Antialiasing, true);

        int outerRadius = 70;
        int innerRadius = 58;

        QPoint center(w->width() / 2, w->height() / 2);

        // Outer circle (translucent)
        QPen pen(QColor(0, 0, 0, 50));
        pen.setWidth(6);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);

        // Draw smooth ring
        p.drawEllipse(center, outerRadius / 2, outerRadius / 2);
    }
    return false;
}

void MainWindow::CalculateMoves() {
    qDebug() << "Starting Perft Test";

    int depth = 5;  // change as needed
    board b = gameBoard;

    auto start = std::chrono::high_resolution_clock::now();

    long long nodes = perft(depth, b);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    qDebug() << "Perft(" << depth << ") nodes:" << nodes;
    qDebug() << "Time:" << elapsed.count() << "seconds";
}

long long MainWindow::perft(int depth, board& b) {
    if (depth == 0) {
        return 1;
    }

    long long nodes = 0;

    std::vector<Move> moves = moveGenerator.generateLegalMoves(b);

    for (Move& m : moves) {
        Unmove u = b.makeMove(m);

        nodes += perft(depth - 1, b);

        b.unmakeMove(m, u); // IMPORTANT: correct order!
    }

    return nodes;
}

