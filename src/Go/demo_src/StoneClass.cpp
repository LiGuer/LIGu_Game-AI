#include "StoneClass.h"


StoneClass::StoneClass(QWidget* parent) : QWidget(parent) {
    w = parent;

    resize(1000, 1000);
    setAttribute(Qt::WA_TranslucentBackground, true);

    StoneWarn->setStyleSheet("QLabel{background:#FF0000;}");
    StoneWarn->setGeometry(0, 0, 0, 0);

    for (int i = 0; i < 361; i++)
        Stone[i] = new QLabel(this);

    setFocusPolicy(Qt::StrongFocus);

    GoAI::evaluate_result.resize(BOARDNUM, 0);
    GoAI::evaluate_visit. resize(BOARDNUM, 0);
}

/*
 *  鼠标响应
 */
void StoneClass::mousePressEvent(QMouseEvent* e) {
    signed char x, y;
    
    if (e->button() == Qt::LeftButton) {    //左键按下
        x = (e->x() - BoardClass::boardMargin + BoardClass::gridSize / 2) / BoardClass::gridSize;
        y = (e->y() - BoardClass::boardMargin + BoardClass::gridSize / 2) / BoardClass::gridSize;

        Go::State* s_ = Go::nextState(*state, y * BOARDSIZE + x);
        if (s_ == NULL)
            return;

        state = s_;

        displayStone(state->board);
        displayWin();

        if (ai_is_open)
            GoAI::move_pos = y * BOARDSIZE + x;

        StoneWarn->setGeometry(
            BoardClass::boardMargin + x * BoardClass::gridSize - stoneSize / 2,
            BoardClass::boardMargin + y * BoardClass::gridSize - stoneSize / 2,
            4, 4
        );
    }
    else if (e->button() == Qt::RightButton) {

    }
    else if (e->button() == Qt::MiddleButton) {
        if (state->parent != NULL) {  
            Go::State* s = state;
            state = state->parent;
            s->parent = NULL;
            delete s;
        }
        displayStone(state->board);
    }
}

/*
 *  键盘相应
 */
void StoneClass::keyPressEvent(QKeyEvent* event) {

    switch (event->key())    {
    case Qt::Key_A: openAI(); break;
    case Qt::Key_S: aiEvaluate(); break;
    case Qt::Key_D: aiEvaluate_visit(); break;
    case Qt::Key_Q: displayQi(*state); break;
    case Qt::Key_M: displayMark(*state); break;
    case Qt::Key_N: displayNumber(*state); break;
    case Qt::Key_P: {
        QMessageBox pass(QMessageBox::NoIcon, "Confirm Pass?", "Do you want to Pass?",
            QMessageBox::Ok | QMessageBox::Cancel);

        if (pass.exec() == QMessageBox::Ok) {
            Go::State* s_ = Go::nextState(*state, PASS);
            state = s_;

            displayWin();

            if (ai_is_open) {
                GoAI::move_pos = -1;
            }
        }
    } break;
    }
}

void StoneClass::openAI() {  
    static ThreadClass ai;
    static QThread thread;

    if (ai_is_open == 0) {
        ai_is_open = 1;
        ai.s = state;

        QObject::connect(&thread, &QThread::started, &ai, &ThreadClass::openAI);
        ai.moveToThread(&thread);
        thread.start();
    }
    return;
}

/*
 *  显示AI评估结果
 */
void StoneClass::aiEvaluate() {
    static int fg = 0;
    static QLabel** labels = new QLabel * [BOARDNUM];
    static QFont font("Times New Roman", 12, 50);

    if (fg == 0) {
        for (int i = 0; i < BOARDNUM; i++) {
            labels[i] = new QLabel(w);
            labels[i]->setFont(font);
            labels[i]->setAlignment(Qt::AlignCenter);
        }
        fg = 1;
    }

    float maxn = -0x7FFFFFFF;
    int maxi = 0;

    if (fg == 1) {
        GoAI::evaluate_fg = 1;
        QThread::msleep(50);

        for (int i = 0; i < BOARDNUM; i++)
            if (state->mark[i] == -1 && maxn < GoAI::evaluate_result[i]) {
                maxn = GoAI::evaluate_result[i];
                maxi = i;
            } 
    } 

    for (int i = 0; i < BOARDNUM; i++) {
        if ((state->mark[i] == -1) && fg == 1) {
            int x = i % BOARDSIZE,
                y = i / BOARDSIZE,
                v = GoAI::evaluate_result[i] * 100;   

            if(i == maxi)
                labels[i]->setStyleSheet("color:Red");
            else  
                labels[i]->setStyleSheet("color:Blue");

            labels[i]->setText(QString::fromStdString(to_string(v)));
            labels[i]->setGeometry(
                BoardClass::boardMargin + BoardClass::gridSize * (x - 0.36),
                BoardClass::boardMargin + BoardClass::gridSize * (y - 0.35), 30, 30);
            labels[i]->show();
        }
        else
            labels[i]->hide();
    }
    fg = -fg;
}

/*
 *  显示AI评估结果
 */
void StoneClass::aiEvaluate_visit() {
    static int fg = 0;
    static QLabel** labels = new QLabel * [BOARDNUM];
    static QFont font("Times New Roman", 8, 50);

    if (fg == 0) {
        for (int i = 0; i < BOARDNUM; i++) {
            labels[i] = new QLabel(w);
            labels[i]->setFont(font);
            labels[i]->setAlignment(Qt::AlignCenter);
            labels[i]->setStyleSheet("color:Blue");
        }
        fg = 1;
    }

    if (fg == 1) {
        GoAI::evaluate_fg = 1;
        QThread::msleep(50);
    }

    for (int i = 0; i < BOARDNUM; i++) {
        if ((state->mark[i] == -1) && fg == 1) {
            int x = i % BOARDSIZE,
                y = i / BOARDSIZE,
                v = GoAI::evaluate_visit[i];

            labels[i]->setText(QString::fromStdString(to_string(v)));
            labels[i]->setGeometry(
                BoardClass::boardMargin + BoardClass::gridSize * (x - 0.36),
                BoardClass::boardMargin + BoardClass::gridSize * (y - 0.35), 30, 30);
            labels[i]->show();
        }
        else
            labels[i]->hide();
    }
    fg = -fg;
}

/*
 *  显示输赢结果
 */
void StoneClass::displayWin() {
    int win = 0;

    // if win
    if (Go::isTermination(*state)) {
        if (Go::computeReward(*state) == BLACK) 
            win = BLACK;
        else 
            win = WHITE;
    }

    if (win != 0) {
        QFont font("Times New Roman", 50, 50, false);//字体，大小，粗细（50正常），是否斜体
        WinLable->setGeometry(0, 0, 0, 0);
        WinLable->setFont(font);
        WinLable->setGeometry(20, 20, 1000, 100);

        if (win == BLACK)
            WinLable->setText("BLACK Win");
        else if (win == WHITE)
            WinLable->setText("WHITE Win");
    }
}

/*
 *  显示棋子
 */
void StoneClass::displayStone(array<Go::Color, BOARDNUM>& board) {
    int StoneCur = 0;

    for (int i = 0; i < BOARDNUM; i++) {
        if (board[i] != 0) {
            int x = i % BOARDSIZE,
                y = i / BOARDSIZE;

            x = BoardClass::boardMargin + x * BoardClass::gridSize - stoneSize / 2;
            y = BoardClass::boardMargin + y * BoardClass::gridSize - stoneSize / 2;

            Stone[StoneCur]->setGeometry(x, y, stoneSize, stoneSize);

            if (board[i] == BLACK)
                Stone[StoneCur]->setStyleSheet(
                    "QLabel{background:#000000; border-radius: 16px; border:0px solid black;}"
                );
            else
                Stone[StoneCur]->setStyleSheet(
                    "QLabel{background:#FF0000; border-radius: 16px; border:0px solid #FF0000;}"
                );

            Stone[StoneCur]->show();
            StoneCur++;
        }
    }
    for (int i = StoneCur; i < BOARDNUM; i++)
        Stone[i]->hide();
}

/*
 *  显示气数
 */
void StoneClass::displayQi(Go::State& s) {
    static int fg = 0;
    static QLabel** labels = new QLabel * [BOARDNUM];
    static QFont font("Times New Roman", 15, 50);

    if (fg == 0) {
        for (int i = 0; i < BOARDNUM; i++) {
            labels[i] = new QLabel(w);
            labels[i]->setFont(font);
            labels[i]->setStyleSheet("color:White");
            labels[i]->setAlignment(Qt::AlignCenter);
        }
        fg = 1;
    }

    for (int i = 0; i < BOARDNUM; i++) {
        if (s.board[i] != 0 && fg == 1) {
            int x = i % BOARDSIZE,
                y = i / BOARDSIZE;

            labels[i]->setText(QString::fromStdString(to_string(s.qi[s.mark[i]])));
            labels[i]->setGeometry(
                BoardClass::boardMargin + BoardClass::gridSize * (x - 0.36),
                BoardClass::boardMargin + BoardClass::gridSize * (y - 0.35), 30, 30);
            labels[i]->show();
        }
        else {
            labels[i]->hide();
        }
    }
    fg = -fg;
}

/*
 *  显示棋块标记
 */
void StoneClass::displayMark(Go::State& s) {
    static int fg = 0;
    static QLabel** labels = new QLabel * [BOARDNUM];
    static QFont font("Times New Roman", 15, 50);

    if (fg == 0) {
        for (int i = 0; i < BOARDNUM; i++) {
            labels[i] = new QLabel(w);
            labels[i]->setFont(font);  
            labels[i]->setAlignment(Qt::AlignCenter);
        }
        fg = 1;
    }

    for (int i = 0; i < BOARDNUM; i++) {
        if (s.mark[i] >= 0 && fg == 1) {
            int x = i % BOARDSIZE,
                y = i / BOARDSIZE;

            labels[i]->setText(QString::fromStdString(to_string(s.mark[i])));
            labels[i]->setGeometry(
                BoardClass::boardMargin + BoardClass::gridSize * (x - 0.36),
                BoardClass::boardMargin + BoardClass::gridSize * (y - 0.35), 30, 30);
            labels[i]->setStyleSheet("color:White");
            labels[i]->show();
        }
        else {
            labels[i]->hide();
        }
    }
    fg = -fg;
}
/*
 *  显示手数
 */
void StoneClass::displayNumber(Go::State& s) {
    static int fg = 0, number[BOARDNUM];
    static QLabel** labels = new QLabel * [BOARDNUM];
    static QFont font("Times New Roman", 15, 50);

    if (fg == 0) {
        for (int i = 0; i < BOARDNUM; i++) {
            labels[i] = new QLabel(w);
            labels[i]->setFont(font);
            labels[i]->setStyleSheet("color:White");
            labels[i]->setAlignment(Qt::AlignCenter);
        }
        fg = 1;
    }

    memset(number, 0, sizeof(int) * BOARDNUM);
    Go::State* s_ = &s;
    int num = 0, sum = 0;

    while (s_ != NULL) {
        num--;
        sum++;

        if(s.board[s_->action] != 0 && number[s_->action] == 0)
            number[s_->action] = num;

        s_ = s_->parent;
    }

    for (int i = 0; i < BOARDNUM; i++) {
        if (number[i] != 0)
            number[i] += sum;  

        if (s.board[i] != 0 && fg == 1) {
            int x = i % BOARDSIZE,
                y = i / BOARDSIZE;

            labels[i]->setText(QString::fromStdString(to_string(number[i])));
            labels[i]->setGeometry(
                BoardClass::boardMargin + BoardClass::gridSize * (x - 0.36),
                BoardClass::boardMargin + BoardClass::gridSize * (y - 0.35), 30, 30);
            labels[i]->show();
        }
        else {
            labels[i]->hide();
        }
    }
    fg = -fg;
}