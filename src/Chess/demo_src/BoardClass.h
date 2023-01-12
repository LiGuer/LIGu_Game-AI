#ifndef CHESS_DEMO_BOARD_H
#define CHESS_DEMO_BOARD_H

#include <QtWidgets/QMainWindow>  
#include <QPainter>
#include <QWidget>
#include <QLabel>
#include "C:/Users/29753/Desktop/Projects/Games/src/Chess/Chess.h"

class BoardClass : public QWidget
{
public:
    static const short
        gridSize = 60,
        boardMargin = 30;
    short lineNum;
    int windowSize;
    
    QLabel
        ** xIndexLabel, 
        ** yIndexLabel;

    BoardClass(QWidget* parent, int _lineNum) : QWidget(parent) {
        lineNum = _lineNum;
        windowSize = (lineNum - 1) * gridSize + boardMargin * 2;
        xIndexLabel = new QLabel * [lineNum];
        yIndexLabel = new QLabel * [lineNum];

        setIndexLabel(this);
    }

    void setIndexLabel(QWidget* widget) {
        //字体，大小，粗细（50正常），是否斜体
        QFont font("Times New Roman", 15, 50, true);

        for (int i = 0; i < BOARDSIZE; i++) {
            xIndexLabel[i] = new QLabel(widget);
            yIndexLabel[i] = new QLabel(widget);

            xIndexLabel[i]->setText(QString((char)('a' + (char)i)));
            yIndexLabel[i]->setText(QString((char)('1' + (char)i)));
            xIndexLabel[i]->setAlignment(Qt::AlignCenter);
            yIndexLabel[i]->setAlignment(Qt::AlignCenter);
            xIndexLabel[i]->setGeometry(boardMargin + gridSize * (i + 0.1), -10, 50, 50);
            yIndexLabel[i]->setGeometry(-10, boardMargin + gridSize * (i), 50, 50);
            xIndexLabel[i]->setFont(font);
            yIndexLabel[i]->setFont(font);
        }
    }

protected:
    void paintEvent(QPaintEvent* event) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        for (int i = 0; i < BOARDSIZE; i++) {
            for (int j = 0; j < BOARDSIZE; j++) {
                if ((i + j) % 2 == 0)
                    painter.fillRect(
                        boardMargin + gridSize * i,
                        boardMargin + gridSize * j,
                        gridSize,
                        gridSize,
                        QBrush(QColor(175, 175, 175, 255))
                    );
                else 
                    painter.fillRect(
                        boardMargin + gridSize * i,
                        boardMargin + gridSize * j,
                        gridSize,
                        gridSize,
                        QBrush(QColor(255, 255, 255, 255))
                    );
            }
        }
    }

};


#endif
