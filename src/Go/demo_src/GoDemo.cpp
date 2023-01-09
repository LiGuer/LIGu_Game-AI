#include "GoDemo.h"

GoDemo::GoDemo(QWidget* parent) : QMainWindow(parent) {
    windowSize = BoardClass::gridSize * (BOARDSIZE + 0.5);
    setMaximumSize(windowSize, windowSize);
    setMinimumSize(windowSize, windowSize);  //固定软件界面的大小
    setWindowTitle("Go");
    setWindowOpacity(0.8);

    // BoardClass 
    setGridWidget(BoardWidget);
    boardClass = new BoardClass(BoardWidget, BOARDSIZE);
    boardClass->setGeometry(0, 0, windowSize, windowSize);

    // StoneClass 
    setStoneWidget(StoneWidget);
    stoneClass = new StoneClass(StoneWidget);
    stoneClass->setGeometry(0, 0, windowSize, windowSize);
    
}

void GoDemo::setGridWidget(QWidget* widget) {
    widget->setGeometry(0, 0, windowSize, windowSize);

    QPalette pal(widget->palette());
    pal.setColor(QPalette::Window, QColor(255, 255, 255));

    widget->setAutoFillBackground(true);
    widget->setPalette(pal);
    widget->show();
}

void GoDemo::setStoneWidget(QWidget* widget) {
    widget->setGeometry(0, 0, windowSize, windowSize);

    QPalette pal(widget->palette());
    pal.setColor(QPalette::Window, QColor(0, 0, 0, 0));

    widget->setAutoFillBackground(true);
    widget->setPalette(pal);
    widget->show();
}