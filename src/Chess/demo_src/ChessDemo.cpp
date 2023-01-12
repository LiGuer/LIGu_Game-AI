#include "ChessDemo.h"

ChessDemo::ChessDemo(QWidget* parent) : QMainWindow(parent) {
    windowSize = BoardClass::gridSize * (BOARDSIZE + 1.2);
    setMaximumSize(windowSize, windowSize);
    setMinimumSize(windowSize, windowSize);  //固定软件界面的大小
    setWindowTitle("Chess");
    setWindowOpacity(0.8);

    // BoardClass 
    setGridWidget(BoardWidget);
    boardClass = new BoardClass(BoardWidget, BOARDSIZE);
    boardClass->setGeometry(0, 0, windowSize, windowSize);

    // ChessClass 
    setChessWidget(ChessWidget);
    chessClass = new ChessClass(ChessWidget);
    chessClass->setGeometry(0, 0, windowSize, windowSize);
    
}

void ChessDemo::setGridWidget(QWidget* widget) {
    widget->setGeometry(0, 0, windowSize, windowSize);

    QPalette pal(widget->palette());
    pal.setColor(QPalette::Window, QColor(255, 255, 255));

    widget->setAutoFillBackground(true);
    widget->setPalette(pal);
    widget->show();
}

void ChessDemo::setChessWidget(QWidget* widget) {
    widget->setGeometry(0, 0, windowSize, windowSize);

    QPalette pal(widget->palette());
    pal.setColor(QPalette::Window, QColor(0, 0, 0, 0));

    widget->setAutoFillBackground(true);
    widget->setPalette(pal);
    widget->show();
}