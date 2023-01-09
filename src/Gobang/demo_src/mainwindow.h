#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "MapModule.h"
#include "ChessModule.h"

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //--------常数--------
    short WindowSize, Mode;

    //--------图层--------
    MapModule *mapModule;
    ChessModule *chessModule;
    QWidget *MapWidget=new QWidget(this);       //网格
    QWidget *ChessWidget=new QWidget(this);     //棋子

    //--------函数--------
    void setGridWidget(QWidget* widget);        //绘制网格界面
    void setChessWidget(QWidget *widget);       //绘制棋子界面
};

#endif // MAINWINDOW_H
