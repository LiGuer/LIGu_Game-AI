#include "mainwindow.h"
//--------结构函数--------
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    printf("Input GameMode (Go/GoBang: 0/1):");
    scanf("%d", &Mode);

    WindowSize = 50 * (Mode == 0 ? 9 : 15);
    setMaximumSize(WindowSize, WindowSize);
    setMinimumSize(WindowSize, WindowSize);//固定软件界面的大小
    setWindowTitle("Daiyu-Go");
    setWindowOpacity(0.8);

    //--------MapModule--------
    setGridWidget(MapWidget);
    mapModule = new MapModule(MapWidget, WindowSize / 50);
    mapModule->setGeometry(0, 0, WindowSize, WindowSize);

    setChessWidget(ChessWidget);
    chessModule = new ChessModule(ChessWidget, Mode);
    mapModule->setGeometry(0, 0, WindowSize, WindowSize);
}

MainWindow::~MainWindow() {
    ;
}

//--------界面生成--------
void MainWindow::setGridWidget(QWidget* widget)
{
    widget->setGeometry(0, 0, WindowSize, WindowSize);
    QPalette pal(widget->palette());
    pal.setColor(QPalette::Window, QColor(255, 255, 255));
    widget->setAutoFillBackground(true);
    widget->setPalette(pal);
    widget->show();
}

void MainWindow::setChessWidget(QWidget* widget)
{
    widget->setGeometry(0, 0, WindowSize, WindowSize);
    QPalette pal(widget->palette());
    pal.setColor(QPalette::Window, QColor(0, 0, 0, 0));
    widget->setAutoFillBackground(true);
    widget->setPalette(pal);
    widget->show();
}

