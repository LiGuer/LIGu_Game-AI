#include "GoDemo.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    GoDemo w;
    w.show();
    return a.exec();
}
