#ifndef CHESSMODULE_H
#define CHESSMODULE_H
#include <QMainWindow>
#include <QPainter>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include "core-src/go.h"
class ChessModule: public QWidget
{
public:
    ChessModule(QWidget *parent);
protected:
    void mousePressEvent(QMouseEvent *e);//鼠标点击事件
private:
    const int ChessSize=25;
    QLabel* Chess[361];
    QLabel* ChessWarn=new QLabel(this);
    QLabel* WinLable=new QLabel(this);
    Go *go=new Go;

    void PrintChess(INT8S* map);
};

#endif // CHESSMODULE_H
