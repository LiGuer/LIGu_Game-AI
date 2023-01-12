#ifndef CHESS_DEMO_H
#define CHESS_DEMO_H

#include <QtWidgets/QMainWindow>  
#include <QPainter>
#include <QWidget>
#include <QLabel>
#include "BoardClass.h"
#include "ChessClass.h"

class ChessDemo : public QMainWindow
{
public:
    short windowSize;

    ChessDemo(QWidget* parent = 0);
    ~ChessDemo() { ; };

    // 图层  
    BoardClass* boardClass;
    ChessClass* chessClass;
    QWidget* BoardWidget = new QWidget(this);     //网格
    QWidget* ChessWidget = new QWidget(this);     //棋子

    // 函数 
    void setGridWidget (QWidget* widget);        //绘制网格界面
    void setChessWidget(QWidget* widget);       //绘制棋子界面
};

#endif