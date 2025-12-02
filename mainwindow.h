#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"

#include "board.h"
#include "movegenerator.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QShortcut>
#include <QListWidget>
#include <QPainter>
#include <QPen>
#include <chrono>
#include <string>
#include "engine.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    void handleTileClick();
    void updateBoardUI();
    void resetColors();
    void undoMove();
    void redoMove();
    void highlightMoves(std::vector<Move>& moves);
    std::vector<QWidget*> highlightOverlays;
    void clearHighlights();
    bool eventFilter(QObject* obj, QEvent* event);

    void CalculateMoves();
    long long perft(int depth, board& b);



private:

    Ui::mainwindowClass ui;
    QPushButton* boardButtons[8][8];  // 2D grid of buttons
    QLabel* turnLabel = NULL;
    QListWidget* moveHistoryList = nullptr;


    board gameBoard;
    MoveGenerator moveGenerator;
    int selectedSquare;
    bool pieceSelected = false;

    Engine engine;

};