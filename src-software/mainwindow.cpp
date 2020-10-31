#include "mainwindow.h"
//==========结构函数==========
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    setMaximumSize(WindowSize[0],WindowSize[1]);
    setMinimumSize(WindowSize[0],WindowSize[1]);//固定软件界面的大小
    setWindowTitle("LiguのGo");
    setWindowOpacity(0.8);
    //======MapModule======
    setGridWidget(MapWidget);
    mapModule = new MapModule(MapWidget);
    mapModule->setGeometry(0,0,WindowSize[0],WindowSize[1]);

    setChessWidget(ChessWidget);
    chessModule = new ChessModule(ChessWidget);
    mapModule->setGeometry(0,0,WindowSize[0],WindowSize[1]);
}
MainWindow::~MainWindow(){}
//==========界面生成==========
void MainWindow::setGridWidget(QWidget *widget)
{
    widget->setGeometry(0,0,WindowSize[0],WindowSize[1]);
    QPalette pal(widget->palette());
    pal.setColor(QPalette::Background,QColor(255,255,255));
    widget->setAutoFillBackground(true);
    widget->setPalette(pal);
    widget->show();
}
void MainWindow::setChessWidget(QWidget *widget)
{
    widget->setGeometry(0,0,WindowSize[0],WindowSize[1]);
    QPalette pal(widget->palette());
    pal.setColor(QPalette::Background,QColor(0,0,0,0));
    widget->setAutoFillBackground(true);
    widget->setPalette(pal);
    widget->show();
}

