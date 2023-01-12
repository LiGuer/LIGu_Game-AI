#include "ChessClass.h"


ChessClass::ChessClass(QWidget* parent) : QWidget(parent)
{
    w = parent;

    resize(1000, 1000);
    setAttribute(Qt::WA_TranslucentBackground, true);

    //ChessWarn->setStyleSheet("QLabel{background:#FF0000;}");
    //ChessWarn->setGeometry(0, 0, 0, 0);

    {
        QFont font("Times New Roman", 40, 50);
        for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
            Chess[i] = new QLabel(this);
            Chess[i]->setAlignment(Qt::AlignCenter);
            Chess[i]->setFont(font);
        }
    }
    {
        QFont font("Arial", 40, 50);
        for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
            ChessEd[i] = new QLabel(this);
            ChessEd[i]->setFont(font);
            ChessEd[i]->setStyleSheet("color:Yellow");
            ChessEd[i]->setAlignment(Qt::AlignCenter);
        }
    }
     
    setFocusPolicy(Qt::StrongFocus);
    Chess::initBoard(state->board);
    Chess::createActionSet(*state);

    printChess(state->board);

    //ChessAI::evaluate_result.resize(BOARDSIZE * BOARDSIZE, 0);
    //ChessAI::evaluate_visit.resize(BOARDSIZE * BOARDSIZE, 0);
}

/*
 *  鼠标响应
 */
void ChessClass::mousePressEvent(QMouseEvent* e) {
    char x, y;
    static int st = -1;
    
    if (e->button() == Qt::LeftButton) {    //左键按下
        y = (e->x() - BoardClass::boardMargin) / BoardClass::gridSize;
        x = (e->y() - BoardClass::boardMargin) / BoardClass::gridSize;

        if (st != -1) {
            int ed = x * BOARDSIZE + y;

            if (st != ed) {
                for (auto e = state->actionSet.begin(); e != state->actionSet.end(); e++) {
                    if (abs(e->first) / 64 == st && abs(e->first) % 64 == ed) {
                        state->action = e->first;

                        Chess::moveChess(*state);
                        break;
                    }
                }
                printChess(state->board);
            }

            st = -1;
            printEd(*state, x * BOARDSIZE + y);
        }
        else {
            if (state->board[x * BOARDSIZE + y] != 0) {
                if (printEd(*state, x * BOARDSIZE + y)) {
                    st = x * BOARDSIZE + y;
                }
            }
        }

        //Chess::moveChess(*state);

        
        //printChess(state->board);

        if (ai_is_open) {
            //ChessAI::move_pos = x * BOARDSIZE + y;
        }
    }/*
    else if (e->button() == Qt::RightButton) {

    }
    else if (e->button() == Qt::MiddleButton) {
        if (state->parent != NULL) {  
            Chess::State* s = state;
            state = state->parent;
            s->parent = NULL;
            delete s;
        }

        printChess(state->board);
    }
    
    // if win
    
    if (Chess::judgeWin(*state) == BLACK) {
        printWin(BLACK);
        return;
    }

    if (Chess::judgeWin(*state) == WHITE) {
        printWin(WHITE);
        return;
    }
    );*/
}

/*
 *  键盘相应
 */
void ChessClass::keyPressEvent(QKeyEvent* event) {
    /*
    switch (event->key())    {
    case Qt::Key_A: openAI(); break;
    case Qt::Key_S: aiEvaluate(); break;
    case Qt::Key_D: aiEvaluate_visit(); break;
    }*/
}

void ChessClass::openAI() {  /*
    static ThreadClass ai;
    static QThread thread;

    if (ai_is_open == 0) {
        ai_is_open = 1;
        ai.s = state;

        QObject::connect(&thread, &QThread::started, &ai, &ThreadClass::openAI);
        ai.moveToThread(&thread);
        thread.start();
    }

    return;*/
}

/*
 *  显示AI评估结果
 */
void ChessClass::aiEvaluate() { /*
    static int fg = 0;
    static QLabel** labels = new QLabel * [BOARDSIZE * BOARDSIZE];
    static QFont font("Times New Roman", 12, 50);

    if (fg == 0) {
        for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
            labels[i] = new QLabel(w);
            labels[i]->setFont(font);
            labels[i]->setAlignment(Qt::AlignCenter);
        }
        fg = 1;
    }

    float maxn = -0x7FFFFFFF;
    int maxi = 0;

    if (fg == 1) {
        ChessAI::evaluate_fg = 1;
        QThread::msleep(50);

        for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++)
            if (state->mark[i] == -1 && maxn < ChessAI::evaluate_result[i]) {
                maxn = ChessAI::evaluate_result[i];
                maxi = i;
            } 
    } 

    for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
        if ((state->mark[i] == -1) && fg == 1) {
            int x = i / BOARDSIZE,
                y = i % BOARDSIZE,
                v = ChessAI::evaluate_result[i] * 100;   

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
        else {
            labels[i]->hide();
        }
    }
    fg = -fg; */
}

/*
 *  显示AI评估结果
 */
void ChessClass::aiEvaluate_visit() { /*
    static int fg = 0;
    static QLabel** labels = new QLabel * [BOARDSIZE * BOARDSIZE];
    static QFont font("Times New Roman", 8, 50);

    if (fg == 0) {
        for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
            labels[i] = new QLabel(w);
            labels[i]->setFont(font);
            labels[i]->setAlignment(Qt::AlignCenter);
            labels[i]->setStyleSheet("color:Blue");
        }
        fg = 1;
    }

    if (fg == 1) {
        ChessAI::evaluate_fg = 1;
        QThread::msleep(50);
    }

    for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
        if ((state->mark[i] == -1) && fg == 1) {
            int x = i / BOARDSIZE,
                y = i % BOARDSIZE,
                v = ChessAI::evaluate_visit[i];

            labels[i]->setText(QString::fromStdString(to_string(v)));
            labels[i]->setGeometry(
                BoardClass::boardMargin + BoardClass::gridSize * (x - 0.36),
                BoardClass::boardMargin + BoardClass::gridSize * (y - 0.35), 30, 30);
            labels[i]->show();
        }
        else {
            labels[i]->hide();
        }
    }
    fg = -fg;*/
}

/*
 *  显示棋子
 */
void ChessClass::printChess(Mat<Chess::Piece>& board) {
    QPalette pe;

    for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
        if (board[i] != 0) {
            int x = i / BOARDSIZE,
                y = i % BOARDSIZE;

            x = BoardClass::boardMargin + (x + 0.05) * BoardClass::gridSize;
            y = BoardClass::boardMargin + (y + 0.05) * BoardClass::gridSize;

            Chess[i]->setGeometry(y, x, BoardClass::gridSize * 0.9, BoardClass::gridSize * 0.9);

            if (board[i] > 0) 
                pe.setColor(QPalette::WindowText, Qt::black);
            else
                pe.setColor(QPalette::WindowText, Qt::red);

            Chess[i]->setPalette(pe);

            switch (abs(board[i])) {
            case Chess::KING:   Chess[i]->setText(QString::fromStdString("♚")); break;
            case Chess::QUEEN:  Chess[i]->setText(QString::fromStdString("♛")); break;
            case Chess::BISHOP: Chess[i]->setText(QString::fromStdString("♝")); break;
            case Chess::ROOK:   Chess[i]->setText(QString::fromStdString("♜")); break;
            case Chess::KNIGHT: Chess[i]->setText(QString::fromStdString("♞")); break;
            case Chess::PAWN:   Chess[i]->setText(QString::fromStdString("♟")); break;
            }

            Chess[i]->show();
        }
        else
            Chess[i]->hide();
    }
}

/*
 *  显示可落子点
 */
bool ChessClass::printEd(Chess::State& s, int st) {
    static int fg = 1;
    int num = 0;

    if (fg == 1) {
        for (auto e = state->actionSet.begin(); e != state->actionSet.end(); e++) {
            if (abs(e->first) / 64 == st) {
                int ed = abs(e->first) % 64,
                    x = ed / BOARDSIZE,
                    y = ed % BOARDSIZE;

                ChessEd[ed]->setText((QString)"〇");
                ChessEd[ed]->setGeometry(
                    BoardClass::boardMargin + BoardClass::gridSize * (y + 0.1),
                    BoardClass::boardMargin + BoardClass::gridSize * (x + 0.1), 50, 50);
                ChessEd[ed]->show();

                num++;
            }
        }
    }
    else {
        for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
            ChessEd[i]->hide();
        }
    }
    
    fg = -fg;

    if (num == 0) {
        fg = 1;
        return false;
    }
    return true;
}

