#ifndef GO_DEMO_STONE_H
#define GO_DEMO_STONE_H

#include <QMainWindow>
#include <QPainter>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QThread> 
#include <QMessageBox>
#include <stdio.h>
#include "../Go.h"
#include "BoardClass.h"
#include "ThreadClass.h"

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
    int ai_is_open = 0;

    QLabel* Stone[361];
    QLabel* StoneWarn = new QLabel(this);
    QLabel* WinLable  = new QLabel(this);

    Go::State* state = new Go::State();

    void openAI();
    void aiEvaluate();
    void aiEvaluate_visit();

    void displayStone(array<Go::Color, BOARDNUM>& Board);
    void displayWin();
    void displayQi(Go::State& s);
    void displayMark(Go::State& s);
    void displayNumber(Go::State& s);
};
#endif