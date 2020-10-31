#ifndef MapModule_H
#define MapModule_H
#include <QMainWindow>
#include <QPainter>
#include <QWidget>
#include <QLabel>

class MapModule : public QWidget
{
public:
    MapModule(QWidget *parent);
    const static short GridSize=50,LineNum = 9,BoardMargin=30;
    int WindowSize = (LineNum-1)*GridSize+BoardMargin*2;
protected:
    void paintEvent(QPaintEvent *event);
private:
    QLabel *XIndexLabel[LineNum],*YIndexLabel[LineNum];
    void setIndexLabel(QWidget *widget);
};
#endif // MapModule_H
