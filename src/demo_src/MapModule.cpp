#include "MapModule.h"

MapModule::MapModule(QWidget* parent, int _LineNum) : QWidget(parent)
{
    LineNum = _LineNum;
    WindowSize = (LineNum - 1) * GridSize + BoardMargin * 2;
    XIndexLabel = new QLabel * [LineNum];
    YIndexLabel = new QLabel * [LineNum];

    setIndexLabel(this);
}

void MapModule::setIndexLabel(QWidget* widget)
{
    QFont font("Times New Roman", 15, 50, true);//字体，大小，粗细（50正常），是否斜体

    for (int i = 0; i < LineNum; i++) {
        char index = 'A' + (char)i;
        XIndexLabel[i] = new QLabel(widget);
        YIndexLabel[i] = new QLabel(widget);
        XIndexLabel[i]->setText(QString(index));
        YIndexLabel[i]->setText(QString(index));

        XIndexLabel[i]->setGeometry(BoardMargin + GridSize * (i - 0.15), 0, 30, 30);
        YIndexLabel[i]->setGeometry(10, BoardMargin + GridSize * (i - 0.3), 30, 30);

        XIndexLabel[i]->setFont(font);
        YIndexLabel[i]->setFont(font);
    }
}

void MapModule::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    for (int i = 0; i < LineNum; i++) {                           //画网格线
        painter.drawLine(
            BoardMargin + GridSize * i, 
            BoardMargin, 
            BoardMargin + GridSize * i, 
            WindowSize - BoardMargin
        );
        painter.drawLine(
            BoardMargin, 
            BoardMargin + GridSize * i,
            WindowSize - BoardMargin, 
            BoardMargin + GridSize * i
        );
    }
    painter.setPen(QPen(4));                            //设置画笔形式
    painter.setBrush(QColor(0, 0, 0));                  //设置画刷，如果不画实现的直接把Brush设置为setBrush(Qt::NoBrush);

    int CenterPos = LineNum / 2, CenterSize = GridSize / 4;     //画中心点
    painter.drawEllipse(
        BoardMargin + GridSize * CenterPos - CenterSize / 2, 
        BoardMargin + GridSize * CenterPos - CenterSize / 2, 
        CenterSize, CenterSize
    );
}

