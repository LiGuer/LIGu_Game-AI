#ifndef CHESSMODULE_H
#define CHESSMODULE_H
#include <QMainWindow>
#include <QPainter>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <stdio.h>

#include "MapModule.h"
#include "core/go.h"
#include "../../../example/Go/GoBangAI.h"

class ChessModule : public QWidget
{
public:
    ChessModule(QWidget* parent, int _Mode);

    Mat<GoBang::CHESS> GoBangBoard;

protected:
    void mousePressEvent(QMouseEvent* e);//鼠标点击事件

private:
    int MapSize = 19;
    const int ChessSize = 25;
    QLabel* Chess[361];
    QLabel* ChessWarn = new QLabel(this);
    QLabel* WinLable = new QLabel(this);

    int Mode;
    Go* go = new Go;

    void PrintChess(INT8S* map);
    void PrintWin(bool win);
};

#endif // CHESSMODULE_H
