#ifndef GO_DEMO_THREAD_H
#define GO_DEMO_THREAD_H

#include <QMainWindow>
#include <QPainter>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QThread> 
#include "../Go_AI_Multithread.h"

class ThreadClass : public QObject {
	Q_OBJECT

public:
	Go::State* s; 

public slots:
	void openAI() {
		GoAI::MonteCarloTreeSearch(s);

		return;
	}
};

#endif
