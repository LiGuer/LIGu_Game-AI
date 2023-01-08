#ifndef GO_DEMO_STONE_H
#define GO_DEMO_STONE_H

#include <QMainWindow>
#include <QPainter>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QKeyEvent>
#include <stdio.h>
#include "C:/Users/29753/Desktop/Projects/Games/src/Go/Go.h"
#include "BoardClass.h"

class StoneClass : public QWidget
{
public:
    StoneClass(QWidget* parent);

protected:
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* e);//鼠标点击事件

private:
    QWidget* w;

    const int stoneSize = BoardClass::gridSize * 0.8;
    QLabel* Stone[361];
    QLabel* StoneWarn = new QLabel(this);
    QLabel* WinLable  = new QLabel(this);

    Go::State* state = new Go::State();

    void printStone(Mat<Go::Stone>& Board);
    void printWin(int win);
    void printQi(Go::State& s);
    void printMark(Go::State& s);
    void printNumber(Go::State& s);
};
#endif