#ifndef GO_DEMO_BOARD_H
#define GO_DEMO_BOARD_H

#include <QtWidgets/QMainWindow>  
#include <QPainter>
#include <QWidget>
#include <QLabel>
#include "C:/Users/29753/Desktop/Projects/Games/src/Go/Go.h"

class BoardClass : public QWidget
{
public:
    static const short
        gridSize = 40,
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

        for (int i = 0; i < lineNum; i++) {
            char index = 'A' + (char)i;

            xIndexLabel[i] = new QLabel(widget);
            yIndexLabel[i] = new QLabel(widget);

            xIndexLabel[i]->setText(QString(index));
            yIndexLabel[i]->setText(QString(index));

            xIndexLabel[i]->setGeometry(boardMargin + gridSize * (i - 0.15), 0, 30, 30);
            yIndexLabel[i]->setGeometry(10, boardMargin + gridSize * (i - 0.3), 30, 30);

            xIndexLabel[i]->setFont(font);
            yIndexLabel[i]->setFont(font);
        }
    }

protected:
    void paintEvent(QPaintEvent* event) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        for (int i = 0; i < lineNum; i++) {                           //画网格线
            painter.drawLine(
                boardMargin + gridSize * i,
                boardMargin,
                boardMargin + gridSize * i,
                windowSize - boardMargin
            );
            painter.drawLine(
                boardMargin,
                boardMargin + gridSize * i,
                windowSize - boardMargin,
                boardMargin + gridSize * i
            );
        }
        painter.setPen(QPen(4));            //设置画笔形式
        painter.setBrush(QColor(0, 0, 0));  //设置画刷，如果不画实现的直接把Brush设置为setBrush(Qt::NoBrush);

        int starSize = gridSize / 4,
            star[][2] = {
                {-6,-6}, {-6, 0}, {-6, 6},
                { 0,-6}, { 0, 0}, { 0, 6},
                { 6,-6}, { 6, 0}, { 6, 6},
            };

        for (int i = 0; i < 9; i++) {
            painter.drawEllipse(
                boardMargin + gridSize * (star[i][0] + BOARDSIZE / 2) - starSize / 2,
                boardMargin + gridSize * (star[i][1] + BOARDSIZE / 2) - starSize / 2,
                starSize, starSize
            );
        }
    }

};


#endif
