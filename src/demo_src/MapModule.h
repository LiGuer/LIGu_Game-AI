#ifndef MapModule_H
#define MapModule_H
#include <QMainWindow>
#include <QPainter>
#include <QWidget>
#include <QLabel>

class MapModule : public QWidget
{
public:
    MapModule(QWidget* parent, int _LineNum);
    static const short GridSize = 50, BoardMargin = 30;
    short LineNum = 9;
    int WindowSize;
    QLabel** XIndexLabel, ** YIndexLabel;

    void setIndexLabel(QWidget* widget);

protected:
    void paintEvent(QPaintEvent* event);

};

#endif // MapModule_H
