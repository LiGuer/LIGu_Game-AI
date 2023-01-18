#ifndef CHESS_DEMO_Chess_H
#define CHESS_DEMO_Chess_H

#include <QMainWindow>
#include <QPainter>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QThread> 
#include <stdio.h>
#include "C:/Users/29753/Desktop/Projects/Games/src/Chess/Chess.h"
#include "BoardClass.h"
#include "ThreadClass.h"

class ChessClass : public QWidget
{
public:
    ChessClass(QWidget* parent);

protected:
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* e);//鼠标点击事件

private:
    QWidget* w;
    int ai_is_open = 0;

    QLabel* Chess[BOARDSIZE * BOARDSIZE];
    QLabel* ChessEd[BOARDSIZE * BOARDSIZE];
    QLabel* ChessWarn = new QLabel(this);
    QLabel* WinLable  = new QLabel(this);

    Chess::State* state = new Chess::State();

    void openAI();
    void aiEvaluate();
    void aiEvaluate_visit();

    void printChess(Mat<Chess::Piece>& Board);
    void printWin(int win);
    bool printEd(Chess::State& s, int st);
    void printOpponentControlPoint(Chess::State& s);
};
#endif