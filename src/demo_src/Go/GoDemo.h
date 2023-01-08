#ifndef GO_DEMO_H
#define GO_DEMO_H

#include <QtWidgets/QMainWindow>  
#include <QPainter>
#include <QWidget>
#include <QLabel>
#include "BoardClass.h"
#include "StoneClass.h"

class GoDemo : public QMainWindow
{
public:
    short windowSize;

    GoDemo(QWidget* parent = 0);
    ~GoDemo() { ; };

    // 图层  
    BoardClass* boardClass;
    StoneClass* stoneClass;
    QWidget* BoardWidget = new QWidget(this);     //网格
    QWidget* StoneWidget = new QWidget(this);     //棋子

    // 函数 
    void setGridWidget (QWidget* widget);        //绘制网格界面
    void setStoneWidget(QWidget* widget);       //绘制棋子界面
};

#endif